// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/DataAssets/ItemDataAsset.h"
#include "ConsumableItem.generated.h"

UCLASS(BlueprintType)
class REFTARKOV_API UConsumableItem : public UItemDataAsset
{
	GENERATED_BODY()

public:
	static const FPrimaryAssetType ItemType;

	virtual FPrimaryAssetType GetItemType() const override { return ItemType; }

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "", meta = (ClampMin = "0.0"))
	float UseTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "")
	TSoftObjectPtr<class USoundBase> UseSound;
	
};
