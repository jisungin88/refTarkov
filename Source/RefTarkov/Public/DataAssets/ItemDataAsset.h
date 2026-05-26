// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"


UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Normal, 
	Rare,
	Epic,
	Legendary,
};

UCLASS(Abstract, BlueprintType)
class REFTARKOV_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetType GetItemType() const
		PURE_VIRTUAL(UItemDataAsset::GetItemType, return FPrimaryAssetType(););

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GetItemType(), GetFName());
	}

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	TSoftObjectPtr<class UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0.0"))
	float WeightKg = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 MaxStackSize = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (AssetRegistrySearchable))
	EItemRarity Rarity = EItemRarity::Normal;
};
