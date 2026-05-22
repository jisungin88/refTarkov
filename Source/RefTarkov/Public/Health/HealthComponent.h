// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

/**
 * Health changed broadcast.
 * @param HealthComp     이 컴포넌트
 * @param OldHealth      변경 전 체력
 * @param NewHealth      변경 후 체력
 * @param InstigatedBy   변경을 유발한 컨트롤러 (힐의 경우 nullptr 가능)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedSignature,
	UHealthComponent*, HealthComp, float, OldHealth, float, NewHealth, AController*, InstigatedBy);

/**
 * Death broadcast.
 * @param HealthComp     이 컴포넌트
 * @param Killer         킬러 컨트롤러 (월드에 의한 죽음이면 nullptr)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeathSignature,
	UHealthComponent*, HealthComp, AController*, Killer);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REFTARKOV_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyHealing(float Amount);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return !bIsDead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const { return MaxHealth > 0 ? CurrentHealth / MaxHealth : 0; }

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnDeathSignature OnDeath;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1"))
	float MaxHealth = 100;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

private:
	UFUNCTION()
	void HandleTakeAnyDamage(
		AActor* DamageActor,
		float Damage,
		const UDamageType* DamageType,
		AController* InstigatedBy,
		AActor* DamageCauser);
};
