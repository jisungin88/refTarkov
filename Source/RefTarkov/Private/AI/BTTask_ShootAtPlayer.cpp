// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ShootAtPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ShooterEnemyCharacter.h"

UBTTask_ShootAtPlayer::UBTTask_ShootAtPlayer()
{
	NodeName = TEXT("Shoot At Player");

	TargetKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTTask_ShootAtPlayer, TargetKey),
		AActor::StaticClass());
}

void UBTTask_ShootAtPlayer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBData = GetBlackboardAsset())
	{
		TargetKey.ResolveSelectedKey(*BBData);
	}
}

EBTNodeResult::Type UBTTask_ShootAtPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
		return EBTNodeResult::Failed;

	AShooterEnemyCharacter* Shooter = Cast<AShooterEnemyCharacter>(AICon->GetPawn());
	if (!Shooter)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
		return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target)
		return EBTNodeResult::Failed;

	const bool bFired = Shooter->PerformRangedAttack(Target);
	return bFired ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
