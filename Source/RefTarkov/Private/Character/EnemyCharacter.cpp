// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"

AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

bool AEnemyCharacter::PerformMeleeAttack()
{
	if (GetWorld()->GetTimeSeconds() - LastMeleeTime < MeleeCooldown)
	{
		return false; // Still on cooldown
	}
	LastMeleeTime = GetWorld()->GetTimeSeconds();

	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * MeleeRange;

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(50.0f);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn, Sphere, Params);
	if (bHit && Hit.GetActor())
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), MeleeDamage, GetController(), this, UDamageType::StaticClass());
	}

	DrawDebugSphere(GetWorld(), End, 50.0f, 12, bHit ? FColor::Red : FColor::Green, false, 1.0f);
	return true;
}