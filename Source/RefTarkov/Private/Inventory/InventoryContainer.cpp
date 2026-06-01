// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryContainer.h"
#include "Inventory/Items/ItemInstance.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

void  UInventoryContainer::Initialize(int32 InMaxSlots, float InMaxWeight)
{
	SetMaxSlots(InMaxSlots);
	MaxWeight = InMaxWeight;
}

bool  UInventoryContainer::TryAddItem(const UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder)
{
	if (!ItemDef || Quantity <= 0)
	{
		OutRemainder = Quantity;
		return false;
	}


	int32 MaxStack = ItemDef->MaxStackSize;
	int32 Remaining = Quantity;
	bool bChanged = false;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].GetItemDef() != ItemDef || Slots[i].StackCount >= MaxStack)
			continue;

		int32 AddCount = FMath::Min(Remaining, MaxStack - Slots[i].StackCount);
		Slots[i].StackCount += AddCount;
		Remaining -= AddCount;
		bChanged = true;
		OnSlotChanged.Broadcast(i);
	
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

	return bChanged;
}

bool  UInventoryContainer::RemoveItemAt(int32 SlotIndex, int32 Quantity)
{
	if (!Slots.IsValidIndex(SlotIndex) || Quantity <= 0 || Slots[SlotIndex].IsEmpty())
		return false;

	int32 RemoveCount = FMath::Min(Quantity, Slots[SlotIndex].StackCount);
	Slots[SlotIndex].StackCount -= RemoveCount;
	if (Slots[SlotIndex].StackCount <= 0)
	{
		Slots[SlotIndex] = FItemInstance();
	}

	OnSlotChanged.Broadcast(SlotIndex);
	OnInventoryChanged.Broadcast();

	return RemoveCount > 0;
}

bool  UInventoryContainer::CanAccept(const UItemDataAsset* ItemDef, int32 Quantity) const
{
	if (!ItemDef || Quantity <= 0)
		return false;

	int32 MaxStack = ItemDef->MaxStackSize;
	int32 AcceptCount = 0;
	
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].IsEmpty())
		{
			AcceptCount += MaxStack;
			continue;
		}

		if (Slots[i].GetItemDef() == ItemDef)
		{
			if (Slots[i].StackCount >= MaxStack)
				continue;

			AcceptCount += MaxStack - Slots[i].StackCount;
		}
	}

	return Quantity <= AcceptCount;
}

float UInventoryContainer::GetTotalWeight() const
{
	float TotalWeight = .0f;
	for (const FItemInstance& Slot : Slots)
	{
		if (Slot.IsEmpty())
			continue;

		if (const UItemDataAsset* ItemDef = Slot.GetItemDef())
		{
			TotalWeight += ItemDef->WeightKg * Slot.StackCount;
		}
	}

	return TotalWeight;
}

int32 UInventoryContainer::GetFreeSlotCount() const
{
	int32 FreeCount = 0;
	for (const FItemInstance& Slot : Slots)
	{
		if (Slot.IsEmpty())
		{
			++FreeCount;
		}
	}

	return FreeCount;
}

bool  UInventoryContainer::SetMaxSlots(int32 NewMaxSlots)
{
	if (NewMaxSlots < 1)
		return false;
	if (NewMaxSlots == Slots.Num())
		return true;

	if (NewMaxSlots < Slots.Num())
	{
		for (int32 i = NewMaxSlots; i < Slots.Num(); ++i)
		{
			if (!Slots[i].IsEmpty())
				return false;
		}
	}

	Slots.SetNum(NewMaxSlots);
	MaxSlots = NewMaxSlots;
	OnInventoryChanged.Broadcast();
	return true;
}

//void UInventoryContainer::SetSort()
//{
//	for (int32 i = 0; i < Slots.Num(); ++i)
//	{
//		for (int32 j = i + 1; j < Slots.Num(); ++j)
//		{
//			if (Slots[i].IsEmpty() || Slots[i].GetItemDef() == Slots[j].GetItemDef())
//			{
//				if (!Slots[j].IsEmpty())
//				{
//					if (Slots[i].IsEmpty())
//					{
//						Slots[i] = Slots[j];
//						Slots[j] = FItemInstance();
//					}
//					else
//					{
//						int32 MaxStack = Slots[i].GetItemDef()->MaxStackSize;
//						int32 AddCount = FMath::Min(Slots[j].StackCount, MaxStack - Slots[i].StackCount);
//						Slots[i].StackCount += AddCount;
//						Slots[j].StackCount -= AddCount;
//						if (Slots[j].StackCount <= 0)
//						{
//							Slots[j] = FItemInstance();
//						}
//					}
//				}
//			}
//		}
//	}
//
//	Slots.Sort([](const FItemInstance& A, const FItemInstance& B)
//	{
//		if (A.IsEmpty() && B.IsEmpty())
//			return false;
//		if (A.IsEmpty())
//			return false;
//		if (B.IsEmpty())
//			return true;
//		//return A.GetItemDef()->GetFName().LexicalLess(B.GetItemDef()->GetFName());
//	});
//	
//	OnInventoryChanged.Broadcast();
//}