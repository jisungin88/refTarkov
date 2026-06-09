// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ShooterEnemyCharacter.h"
#include "Weapons/WeaponBase.h"

void AShooterEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetActorTransform(), Params);
		if (CurrentWeapon)
		{
			const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
			CurrentWeapon->AttachToComponent(GetMesh(), Rules, TEXT("hand_r"));
		}
	}
}

bool AShooterEnemyCharacter::PerformRangedAttack(AActor* Target)
{
	if (!CurrentWeapon || !Target)
	{
		return false;
	}

	// AI는 발사 입력이 없으니 탄 0이면 여기서 직접 재장전
	if (CurrentWeapon->GetCurrentAmmoInMag() <= 0)
	{
		CurrentWeapon->StartReload();
		return false;
	}

	CurrentWeapon->Fire(Target->GetActorLocation());
	return true;
}
