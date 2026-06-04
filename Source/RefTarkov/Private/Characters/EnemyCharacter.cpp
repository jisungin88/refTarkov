// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"
#include "MinionsPlayerState.h"
#include "Components/RaidStatsComponent.h"


AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TeamId = 1; // 적 팀
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

void AEnemyCharacter::HandleDeath(UHealthComponent* HealthComponent, AController* Killer)
{
	// [역할] 기반 클래스의 래그돌/파괴 처리를 먼저 실행한다.
	//        그 다음, 킬러가 플레이어 컨트롤러일 때만 킬 카운트를 기록한다.
	//        AI끼리 피해를 주고받을 때는 카운트되면 안 된다.
	Super::HandleDeath(HealthComponent, Killer);

	if (APlayerController* PC = Cast<APlayerController>(Killer))
	{
		if (AMinionsPlayerState* PS = PC->GetPlayerState<AMinionsPlayerState>())
		{
			PS->RaidStats->RecordKill();
		}
	}
}
