// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotChanged, 
	int32, SlotIndex);

class FItemInstance;

UCLASS(BlueprintType)
class REFTARKOV_API UInventoryContainer : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TArray<FItemInstance> Slots;

	int32 MaxSlots = 12;
	float MaxWeight = -1.f;

public:
	void Initialize(int32 InMaxSlots, float InMaxWeight = -1.f);

	bool TryAddItem(const UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder);
	bool RemoveItemAt(int32 SlotIndex, int32 Quantity);
	bool CanAccept(const UItemDataAsset* ItemDef, int32 Quantity) const;

	float GetTotalWeight() const;
	int32 GetFreeSlotCount() const;
	int32 GetMaxSlotCount() const { return MaxSlots; }
	float GetMaxWeightKg() const { return MaxWeight; }

	const TArray<FItemInstance>& GetSlots() const { return Slots; }

	bool SetMaxSlots(int32 NewMaxSlots);
	//void SetSort();

	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSlotChanged OnSlotChanged;
};
