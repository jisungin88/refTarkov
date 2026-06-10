// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_EvaluateSuspicion.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"

UBTTask_EvaluateSuspicion::UBTTask_EvaluateSuspicion()
{
	NodeName = TEXT("Evaluate Suspicion");
	bNotifyTick = true;

	SuspicionTargetKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTTask_EvaluateSuspicion, SuspicionTargetKey),
		AActor::StaticClass());
}

uint16 UBTTask_EvaluateSuspicion::GetInstanceMemorySize() const
{
	return sizeof(FTaskMemory);
}

EBTNodeResult::Type UBTTask_EvaluateSuspicion::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB)
		return EBTNodeResult::Failed;

	AActor* SuspicionActor = Cast<AActor>(BB->GetValueAsObject(SuspicionTargetKey.SelectedKeyName));
	if (!SuspicionActor)
		return EBTNodeResult::Failed;

	AIC->SetFocus(SuspicionActor);

	reinterpret_cast<FTaskMemory*>(NodeMemory)->ElapsedTime = 0.f;
	return EBTNodeResult::InProgress;
}

void UBTTask_EvaluateSuspicion::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FTaskMemory* Memory = reinterpret_cast<FTaskMemory*>(NodeMemory);
	Memory->ElapsedTime += DeltaSeconds;

	AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const AEnemyAIController* EnemyAIC = Cast<AEnemyAIController>(AIC);
	const float Duration = EnemyAIC ? EnemyAIC->SuspicionDuration : 1.5f;

	if (Memory->ElapsedTime > Duration)
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);

		if (AActor* SuspicionActor = Cast<AActor>(BB->GetValueAsObject(SuspicionTargetKey.SelectedKeyName)))
		{
			BB->SetValueAsObject(AEnemyAIController::TargetActorKey, SuspicionActor);
		}
		BB->ClearValue(SuspicionTargetKey.SelectedKeyName);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_EvaluateSuspicion::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIC = OwnerComp.GetAIOwner())
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->ClearValue(SuspicionTargetKey.SelectedKeyName);
	}

	return EBTNodeResult::Aborted;
}
