// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatDirectorSubsystem.generated.h"

class AEnemyAIController;

UENUM(BlueprintType)
enum class ECombatRole : uint8
{
	Free,
	Suppress,
	Flank
};

UCLASS()
class REFTARKOV_API UCombatDirectorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterCombatant(AEnemyAIController* Controller);
	void UnregisterCombatant(AEnemyAIController* Controller);

private:
	void AssignRoles();
	void SetRole(AEnemyAIController* Controller, ECombatRole Role);

	TArray<TWeakObjectPtr<AEnemyAIController>> ActiveCombatants;
};
