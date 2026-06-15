// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CombatDirectorSubsystem.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UCombatDirectorSubsystem::RegisterCombatant(AEnemyAIController* Controller)
{
	if (!Controller)
		return;

	ActiveCombatants.AddUnique(Controller);
	AssignRoles();
}

void UCombatDirectorSubsystem::UnregisterCombatant(AEnemyAIController* Controller)
{
	ActiveCombatants.RemoveAll([Controller](const TWeakObjectPtr<AEnemyAIController>& Ptr)
	{
		return !Ptr.IsValid() || Ptr.Get() == Controller;
	});
	AssignRoles();
}

void UCombatDirectorSubsystem::AssignRoles()
{
	TArray<AEnemyAIController*> Valid;
	for (const TWeakObjectPtr<AEnemyAIController>& Ptr : ActiveCombatants)
	{
		if (Ptr.IsValid())
			Valid.Add(Ptr.Get());
	}

	if (Valid.Num() == 0)
		return;

	if (Valid.Num() == 1)
	{
		SetRole(Valid[0], ECombatRole::Free);
		return;
	}

	for (int32 i = 0; i < Valid.Num() - 1; ++i)
		SetRole(Valid[i], ECombatRole::Suppress);

	SetRole(Valid.Last(), ECombatRole::Flank);
}

void UCombatDirectorSubsystem::SetRole(AEnemyAIController* Controller, ECombatRole Role)
{
	if (!Controller)
		return;

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if (!Blackboard)
		return;

	Blackboard->SetValueAsInt(AEnemyAIController::CombatRoleKey, static_cast<int32>(Role));
}
