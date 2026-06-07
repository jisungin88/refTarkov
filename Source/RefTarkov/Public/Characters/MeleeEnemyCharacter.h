// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacter.h"
#include "MeleeEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class REFTARKOV_API AMeleeEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformMeleeAttack();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float MeleeRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float MeleeDamage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.1"))
	float MeleeCooldown = 1.f;

	float LastMeleeTime = -FLT_MAX;
};
