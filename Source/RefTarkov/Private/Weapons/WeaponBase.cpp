// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/WeaponBase.h"
#include "Inventory/DataAssets/WeaponDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Point"));
	MuzzlePoint->SetupAttachment(WeaponMesh);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	// ensure 실패해도 게임은 계속 돌아간다(크래시 X), 다만 발사는 안 됨.
	if (!ensureMsgf(WeaponData, TEXT("AWeaponBase %s has no WeaponData assigned"), *GetName()))
	{
		return;
	}

	CurrentAmmoInMag = WeaponData->MagSize;
}

bool AWeaponBase::CanFire() const 
{
	if (!WeaponData) return false;
	if (bIsReloading) return false;
	if (CurrentAmmoInMag <= 0)
	{
//		StartReload();
		return false;
	}

	const float FireInterval = 60.f / FMath::Max(1, WeaponData->RPM);
	//UE_LOG(LogTemp, Warning, TEXT("Fire Time: %.1f / %.1f"), GetWorld()->GetTimeSeconds() - LastFireTime, FireInterval)
	if (GetWorld()->GetTimeSeconds() - LastFireTime < FireInterval)
		return false;

	return true;
}

void AWeaponBase::Fire(const FVector& TargetLocation)
{
	if (!CanFire())
		return;

	LastFireTime = GetWorld()->GetTimeSeconds();
	--CurrentAmmoInMag;
	OnAmmoChanged.Broadcast(CurrentAmmoInMag);

	FVector Start = MuzzlePoint->GetComponentLocation();
	FVector Aim = TargetLocation;
	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		float ChestZ = OwnerChar->GetActorLocation().Z + 30;
		Start.Z = ChestZ;
		Aim.Z = ChestZ;
	}

	const FVector Direction = (Aim - Start).GetSafeNormal();
	const FVector End = Start + Direction * WeaponData->Range;
	PerformFireTrace(Start, End);

	UAISense_Hearing::ReportNoiseEvent(
		GetWorld(),
		GetActorLocation(),   // 소음 발생 위치 (총구)
		1.0f,                 // Loudness (0~1)
		GetOwner(),           // 소음 유발 Actor
		0.f,                  // Max Range (0 = HearingConfig.HearingRange 사용)
		NAME_None
	);
}

void AWeaponBase::PerformFireTrace(const FVector& Start, const FVector& End)
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	const FVector ImpactPoint = bHit ? HitResult.ImpactPoint : End;
	if (bHit && HitResult.GetActor())
	{
		AController* OwnerController = nullptr;
		if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
		{
			OwnerController = OwnerPawn->GetController();
		}

		UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponData->BaseDamage,
			OwnerController, this, UDamageType::StaticClass());
	}

	DrawDebugLine(GetWorld(), Start, ImpactPoint, 
		bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 2);
}

void AWeaponBase::StartReload()
{
	if (!WeaponData) return;
	if (bIsReloading) return;
	if (CurrentAmmoInMag >= WeaponData->MagSize) return;

	bIsReloading = true;

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AWeaponBase::FinishReload,
		WeaponData->ReloadTime,
		false);
}

void AWeaponBase::FinishReload()
{
	if (!WeaponData) return;

	// 임시
	CurrentAmmoInMag = WeaponData->MagSize;
	OnAmmoChanged.Broadcast(CurrentAmmoInMag);

	bIsReloading = false;
}
