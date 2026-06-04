// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Items/ItemInstance.h"
#include "InventoryComponent.generated.h"

class UItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedToInventory,
	UItemDataAsset*, ItemDef, int32, Quantity);

UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class REFTARKOV_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 12;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "0.0"))
	float MaxWeightKg = 30;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class UInventoryContainer> Container;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItem(const UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanAccept(const UItemDataAsset* ItemDef, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemAt(int32 SlotIndex, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetTotalWeight() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetFreeSlotCount() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FItemInstance>& GetSlots() const;

	// Container 직접 접근 — 델리게이트 구독 등에 필요
	UInventoryContainer* GetContainer() const { return Container; }

	UPROPERTY(BlueprintAssignable,Category = "Inventory")
	FOnItemAddedToInventory OnItemAdded;
};
