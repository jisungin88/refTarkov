// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacter.h"
#include "ShooterEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class REFTARKOV_API AShooterEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformRangedAttack(AActor* Target);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;
};
