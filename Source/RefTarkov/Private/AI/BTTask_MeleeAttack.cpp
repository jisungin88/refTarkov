// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MeleeAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/EnemyAIController.h"
#include "Characters/MeleeEnemyCharacter.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
	bNotifyTick = true;
}

uint16 UBTTask_MeleeAttack::GetInstanceMemorySize() const
{
	return sizeof(FTaskMemory);
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
		return EBTNodeResult::Failed;

	AMeleeEnemyCharacter* Enemy = Cast<AMeleeEnemyCharacter>(AICon->GetPawn());
	if (!Enemy)
		return EBTNodeResult::Failed;

	FTaskMemory* Memory = reinterpret_cast<FTaskMemory*>(NodeMemory);

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		if (AActor* Target = Cast<AActor>(BB->GetValueAsObject(AEnemyAIController::TargetActorKey)))
		{
			FVector ToTarget = Target->GetActorLocation() - Enemy->GetActorLocation();
			ToTarget.Z = 0.f;
			Memory->TargetRotation = ToTarget.IsNearlyZero()
				? Enemy->GetActorRotation()
				: ToTarget.Rotation();
		}
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AMeleeEnemyCharacter* Enemy = Cast<AMeleeEnemyCharacter>(AICon->GetPawn());
	if (!Enemy)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FTaskMemory* Memory = reinterpret_cast<FTaskMemory*>(NodeMemory);

	const FRotator NewRot = FMath::RInterpConstantTo(
		Enemy->GetActorRotation(), Memory->TargetRotation, DeltaSeconds, RotationSpeed);
	Enemy->SetActorRotation(NewRot);

	const float AngleDiff = FMath::Abs(
		FMath::FindDeltaAngleDegrees(NewRot.Yaw, Memory->TargetRotation.Yaw));

	if (AngleDiff < 5.f)
	{
		Enemy->SetActorRotation(Memory->TargetRotation);
		Enemy->PerformMeleeAttack();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_MeleeAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}
