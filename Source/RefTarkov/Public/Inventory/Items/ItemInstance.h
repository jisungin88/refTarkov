// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.generated.h"

USTRUCT(BlueprintType)
struct REFTARKOV_API FItemInstance
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<class UItemDataAsset> ItemDef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 StackCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	int32 LoadedAmmo = -1;

	bool IsEmpty() const { return ItemDef == nullptr || StackCount <= 0; }

	const UItemDataAsset* GetItemDef() const { return ItemDef; }

};
