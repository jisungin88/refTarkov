// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIAlertSubsystem.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UAIAlertSubsystem::RegisterEnemy(AEnemyAIController* Controller)
{
	if (Controller)
		RegisteredEnemies.AddUnique(Controller);
}

void UAIAlertSubsystem::UnregisterEnemy(AEnemyAIController* Controller)
{
	RegisteredEnemies.RemoveAll([Controller](const TWeakObjectPtr<AEnemyAIController>& Ptr)
	{
		return !Ptr.IsValid() || Ptr.Get() == Controller;
	});
}

void UAIAlertSubsystem::BroadcastAlert(FVector ThreatLocation, float AlertRadius, uint8 SourceTeamId, AEnemyAIController* Instigator)
{
	const float RediusSq = FMath::Square(AlertRadius);
	for (const TWeakObjectPtr<AEnemyAIController>& Ptr : RegisteredEnemies)
	{
		AEnemyAIController* AIC = Ptr.Get();
		if (!AIC || AIC == Instigator || !AIC->GetPawn())
			continue;

		if (AIC->GetGenericTeamId().GetId() != SourceTeamId)
			continue;

		if (FVector::DistSquared(AIC->GetPawn()->GetActorLocation(), ThreatLocation) < RediusSq)
			continue;

		UBlackboardComponent* BB = AIC->GetBlackboardComponent();
		if (!BB)
			continue;

		if (BB->GetValueAsObject(AEnemyAIController::TargetActorKey))
			continue;
		if (BB->GetValueAsObject(AEnemyAIController::SuspicionTargetKey))
			continue;

		DrawDebugSphere(GetWorld(), ThreatLocation, AlertRadius, 16,
				  FColor::Orange, false, 3.f);

		BB->SetValueAsVector(AEnemyAIController::InvestigateLocationKey, ThreatLocation);

		UE_LOG(LogTemp, Warning, TEXT("[Alert] %s →InvestigateLocation 설정됨"),
		 *AIC->GetPawn()->GetName());
	}
}
