// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryComponent.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Slots.SetNum(MaxSlots);
}

bool UInventoryComponent::TryAddItem(const UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder)
{
	return Container ? Container->TryAddItem(ItemDef, Quantity, OutRemainder) : false;
	
	/*if (ItemDef == nullptr || Quantity <= 0)
	{
		OutRemainder = Quantity;
		return false;
	}

	const int32 MaxStack = ItemDef->MaxStackSize;
	int32 Remaining = Quantity;
	bool bChanged = false;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].GetItemDef() == ItemDef)
		{
			if (Slots[i].StackCount >= ItemDef->MaxStackSize)
				continue;

			int32 AddCount = FMath::Min(Remaining, MaxStack - Slots[i].StackCount);
			Remaining -= AddCount;
			Slots[i].StackCount += AddCount;
			bChanged = true;
			OnSlotChanged.Broadcast(i);
		}

		if (Remaining <= 0)
			break;
	}

	if (Remaining > 0)
	{
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (!Slots[i].IsEmpty())
				continue;

			int32 AddCount = FMath::Min(Remaining, MaxStack);

			Slots[i].ItemDef = const_cast<UItemDataAsset*>(ItemDef);
			Slots[i].StackCount = AddCount;

			Remaining -= AddCount;
			bChanged = true;
			OnSlotChanged.Broadcast(i);

			if (Remaining <= 0)
				break;
		}
	}

	OutRemainder = Remaining;
	if (bChanged)
	{
		OnInventoryChanged.Broadcast();
	}

	return bChanged;*/
}

bool UInventoryComponent::CanAccept(const UItemDataAsset* ItemDef, int32 Quantity) const
{
	if (ItemDef == nullptr || Quantity <= 0)
		return false;

	int32 MaxCount = ItemDef->MaxStackSize;
	int32 AcceptCount = 0;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].IsEmpty())
		{
			AcceptCount += MaxCount;
			continue;
		}

		if (Slots[i].GetItemDef() == ItemDef)
		{
			if (Slots[i].StackCount >= MaxCount)
				continue;

			AcceptCount += MaxCount - Slots[i].StackCount;
		}
	}

	return Quantity <= AcceptCount;
}

bool UInventoryComponent::RemoveItemAt(int32 SlotIndex, int32 Quantity)
{
	if (!Slots.IsValidIndex(SlotIndex)) return false;
	if (Slots[SlotIndex].IsEmpty()) return false;
	if (Quantity <= 0) return false;

	const int32 Removed = FMath::Min(Quantity, Slots[SlotIndex].StackCount);
	Slots[SlotIndex].StackCount -= Removed;

	if (Slots[SlotIndex].StackCount <= 0)
	{
		Slots[SlotIndex] = FItemInstance();
	}

	OnSlotChanged.Broadcast(SlotIndex);
	OnInventoryChanged.Broadcast();

	return Removed > 0;
}

float UInventoryComponent::GetTotalWeight() const
{
	float Total = .0f;

	for (const FItemInstance& Slot : Slots)
	{
		if (!Slot.IsEmpty())
		{
			Total += Slot.StackCount * Slot.GetItemDef()->WeightKg;
		}
	}

	return Total;
}

int32 UInventoryComponent::GetFreeSlotCount() const
{
	int32 FreeSlotCount = 0;
	for (const FItemInstance& Slot : Slots)
	{
		if (Slot.IsEmpty()) 
		{
			FreeSlotCount++;
		}
	}

	return FreeSlotCount;
}
