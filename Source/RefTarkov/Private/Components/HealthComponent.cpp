// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "GameFramework/Actor.h"
 
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(
	AActor* DamageActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDead || Damage <= 0)
		return;

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, MaxHealth);
	OnHealthChanged.Broadcast(this, OldHealth, CurrentHealth, InstigatedBy);
	if (CurrentHealth <= 0)
	{
		bIsDead = true;
		OnDeath.Broadcast(this, InstigatedBy);
	}
}

void UHealthComponent::ApplyHealing(float Amount)
{
	if (bIsDead || Amount <= 0)
		return;

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
	OnHealthChanged.Broadcast(this, OldHealth, CurrentHealth, nullptr);
}

