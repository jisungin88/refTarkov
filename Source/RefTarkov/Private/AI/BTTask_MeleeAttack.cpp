// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_MeleeAttack.h"
#include "AIController.h"
#include "Character/EnemyCharacter.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) 
		return EBTNodeResult::Failed;

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AICon->GetPawn());
	if (!Enemy) 
		return EBTNodeResult::Failed;

	const bool bAttacked = Enemy->PerformMeleeAttack();
	return bAttacked ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
