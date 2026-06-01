// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/WeaponTypes.h"
#include "WeaponDataAsset.generated.h"

UCLASS(BlueprintType)
class REFTARKOV_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FName WeaponId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing")
	EFireModeType FireMode = EFireModeType::Single;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing",
		meta = (ClampMin = "1", UIMin = "1", UIMax = "1500"))
	int32 RPM = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing",
		meta = (ClampMin = "0.0"))
	float BaseDamage = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Firing",
		meta = (ClampMin = "0.0", UIMax = "20000.0"))
	float Range = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	EAmmoType AmmoType = EAmmoType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo",
		meta = (ClampMin = "1"))
	int32 MagSize = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo",
		meta = (ClampMin = "0.0"))
	float ReloadTime = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Penetration",
		meta = (ClampMin = "0", UIMax = "99"))
	int32 PenetrationPower = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory",
		meta = (ClampMin = "0.0"))
	float WeightKg = 3;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType("Weapon"), WeaponId);
	}
};
