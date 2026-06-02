// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/StashSubsystem.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/Items/ItemInstance.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

void UStashSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Container = NewObject<UInventoryContainer>(this);
	Container->Initialize(InitialMaxSlots);
}

void UStashSubsystem::TransferFromInventory(UInventoryContainer* Source)
{
    if (!Source || !Container || Source == Container) return;

    const TArray<FItemInstance>& SrcSlots = Source->GetSlots();
    const int32 NumSlots = SrcSlots.Num();

    // 아이템 옮기기
    for (int32 i = 0; i < NumSlots; ++i)
    {
        if (SrcSlots[i].IsEmpty()) continue;

        int32 Remainder = 0;
        Container->TryAddItem(SrcSlots[i].GetItemDef(),
            SrcSlots[i].StackCount, Remainder);
        if (Remainder > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Stash overflow: %d of slot %d lost"),
                Remainder, i);
        }
    }

    // Source 비우기 — 옮긴 수량만큼 차감
    for (int32 i = 0; i < NumSlots; ++i)
    {
        if (SrcSlots[i].IsEmpty()) continue;
        Source->RemoveItemAt(i, SrcSlots[i].StackCount);
    }
}

bool UStashSubsystem::TryAddItem(const UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder)
{
    return Container ? Container->TryAddItem(ItemDef, Quantity, OutRemainder) : false;
}

bool UStashSubsystem::RemoveItemAt(int32 SlotIndex, int32 Quantity)
{
    // TODO: 한 줄 forward
	return Container ? Container->RemoveItemAt(SlotIndex, Quantity) : false;
}

const TArray<FItemInstance>& UStashSubsystem::GetSlots() const
{
    // TODO: Container 있으면 Container->GetSlots(), 없으면 static empty 반환
    //   힌트: static const TArray<FItemInstance> Empty;  로컬 static OK
	static const TArray<FItemInstance> Empty;
	return Container ? Container->GetSlots() : Empty;
}

bool UStashSubsystem::ExpandSlots(int32 NewMaxSlots)
{
    // TODO: Container->SetMaxSlots(NewMaxSlots) forward
	return Container ? Container->SetMaxSlots(NewMaxSlots) : false;
}

void UStashSubsystem::Deinitialize()
{
    Container = nullptr;   // GC가 알아서 정리하지만 명시
    Super::Deinitialize();
}