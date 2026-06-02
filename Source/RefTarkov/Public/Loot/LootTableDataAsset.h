// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTableDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FLootEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<class UItemDataAsset> ItemDef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MinQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"))
	int32 MaxQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float Weight = 1.f;
};

UCLASS(BlueprintType)
class REFTARKOV_API ULootTableDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	void RollAndAddTo(class UInventoryContainer* Container) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 MinRolls = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 MaxRolls = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLootEntry> Entries;
};
