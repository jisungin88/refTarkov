// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Container = NewObject<UInventoryContainer>(this);
	Container->Initialize(MaxSlots, MaxWeightKg);
}

bool UInventoryComponent::TryAddItem(const UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder)
{
	if (!Container)
		return false;

	const bool bChanged = Container->TryAddItem(ItemDef, Quantity, OutRemainder);
	if (bChanged)
		OnItemAdded.Broadcast(const_cast<UItemDataAsset*>(ItemDef), Quantity - OutRemainder);

	return bChanged;
}

bool UInventoryComponent::CanAccept(const UItemDataAsset* ItemDef, int32 Quantity) const
{
	return Container ? Container->CanAccept(ItemDef, Quantity) : false;
}

bool UInventoryComponent::RemoveItemAt(int32 SlotIndex, int32 Quantity)
{
	return Container ? Container->RemoveItemAt(SlotIndex, Quantity) : false;
}

float UInventoryComponent::GetTotalWeight() const
{
	return Container ? Container->GetTotalWeight() : 0.f;
}

const TArray<FItemInstance>& UInventoryComponent::GetSlots() const
{
	static const TArray<FItemInstance> Empty;
	return Container ? Container->GetSlots() : Empty;
}

int32 UInventoryComponent::GetFreeSlotCount() const
{
	return Container ? Container->GetFreeSlotCount() : 0;
}
