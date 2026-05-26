// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/ItemInstance.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotChanged, 
	int32, SlotIndex);

UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class REFTARKOV_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnSlotChanged OnSlotChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 MaxSlots = 12;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = "0.0"))
	float MaxWeightKg = 30;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TArray<FItemInstance> Slots;

public:	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItem(const class UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanAccept(const UItemDataAsset* ItemDef, int32 Quantity) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItemAt(int32 SlotIndex, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetTotalWeight() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetFreeSlotCount() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FItemInstance>& GetSlots() const { return Slots; }
};
