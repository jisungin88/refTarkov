// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/DataAssets/ItemDataAsset.h"
#include "Types/WeaponTypes.h"
#include "AmmoItem.generated.h"

UCLASS(BlueprintType)
class REFTARKOV_API UAmmoItem : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	static const FPrimaryAssetType ItemType;
	virtual FPrimaryAssetType GetItemType() const override { return ItemType; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	EAmmoType AmmoType = EAmmoType::Pistol_9mm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0"))
	int32 PenetrationPower = 0;
};
