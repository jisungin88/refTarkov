// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/LootTableDataAsset.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

void ULootTableDataAsset::RollAndAddTo(class UInventoryContainer* Container) const
{
	if (!Container || Entries.Num() == 0)
		return;
	
	float TotalWeight = 0.f;
	for (const FLootEntry& Entry : Entries)
	{
		TotalWeight += Entry.Weight;
	}
	if (TotalWeight <= 0.f)
		return;

	const int32 RollCount = FMath::RandRange(MinRolls, MaxRolls);
	for (int32 i = 0; i < RollCount; ++i)
	{
		float Roll = FMath::FRandRange(0.f, TotalWeight);
		for (const FLootEntry& Entry : Entries)
		{
			if (Roll <= Entry.Weight)
			{
				if (UItemDataAsset* ItemDef = Entry.ItemDef.LoadSynchronous())
				{
					const int32 Quantity = FMath::RandRange(Entry.MinQuantity, Entry.MaxQuantity);
					int32 Remainder = 0;
					Container->TryAddItem(ItemDef, Quantity, Remainder);
					if (Remainder > 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("Could not add all items to container. Remainder: %d"), Remainder);
					}
				}
				break;
			}
			Roll -= Entry.Weight;
		}
	}
}
