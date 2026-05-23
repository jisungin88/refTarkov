// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MinionCharacterBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class REFTARKOV_API AEnemyCharacter : public AMinionCharacterBase
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformMeleeAttack();

	UBehaviorTree* GetBehaviorTree() const { return BehaviorTreeAsset; }

protected:
	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float MeleeRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float MeleeDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.1"))
	float MeleeCooldown = 1.0f;

	float LastMeleeTime = -FLT_MAX;
};
