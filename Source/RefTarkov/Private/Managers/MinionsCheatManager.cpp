// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/MinionsCheatManager.h"
#include "Inventory/DataAssets/ItemDataAsset.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/Items/ItemInstance.h"              
#include "GameFramework/PlayerController.h"  

void UMinionsCheatManager::DebugAddItem(FName Name, int32 Quantity)
{
    UInventoryComponent* Inven = GetTargetInventory();
    if (!Inven)
    { 
        UE_LOG(LogTemp, Warning, TEXT("No Inventory")); 
        return; 
    }

    TSoftObjectPtr<UItemDataAsset>* Found = DebugItemTable.Find(Name);
    if (!Found || Found->IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Cheat] Item '%s' not registered in DebugItemTable"), *Name.ToString());
        return;
    }

    UItemDataAsset* ItemDef = Found->LoadSynchronous();
    if (!ItemDef)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Cheat] Failed to load asset for '%s'"), *Name.ToString());
        return;
    }

    int32 Remainder = 0;
    const bool bResult = Inven->TryAddItem(ItemDef, Quantity, Remainder);

    UE_LOG(LogTemp, Display, TEXT("[Cheat] AddItem %s x%d → %s (added: %d, remainder: %d)"),
        *Name.ToString(),
        Quantity,
        bResult ? TEXT("OK") : TEXT("FAIL"),
        Quantity - Remainder,
        Remainder);
}

void UMinionsCheatManager::DebugRemoveItemAt(int32 SlotIndex, int32 Quantity)
{
    UInventoryComponent* Inven = GetTargetInventory();
    if (!Inven)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Inventory"));
        return;
    }

    const bool bResult = Inven->RemoveItemAt(SlotIndex, Quantity);
    UE_LOG(LogTemp, Display, TEXT("[Cheat] RemoveAt Slot:%d x%d → %s"),
        SlotIndex,
        Quantity,
        bResult ? TEXT("OK") : TEXT("FAIL"));
}

void UMinionsCheatManager::DebugDumpInventory()
{
    UInventoryComponent* Inven = GetTargetInventory();
    if (!Inven)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Inventory"));
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("=== Inventory Dump ==="));
    UE_LOG(LogTemp, Display, TEXT("Weight: %.2fkg | Free Slots: %d"),
        Inven->GetTotalWeight(), Inven->GetFreeSlotCount());

    const TArray<FItemInstance>& Slots = Inven->GetSlots();
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].IsEmpty())
        {
            UE_LOG(LogTemp, Display, TEXT("[%d] <empty>"), i);
            continue;
        }

        UE_LOG(LogTemp, Display, TEXT("[%d]%s:%s, %.1f, %d, %s"), i,
			*Slots[i].ItemDef->DisplayName.ToString(),
			*Slots[i].ItemDef->Description.ToString(),
            Slots[i].ItemDef->WeightKg,
			Slots[i].ItemDef->MaxStackSize,
			*StaticEnum<EItemRarity>()->GetValueAsString(Slots[i].ItemDef->Rarity)
        );
        
    }
}

class UInventoryComponent* UMinionsCheatManager::GetTargetInventory() const
{
    APlayerController* PC = GetOuterAPlayerController();
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;
    return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}
