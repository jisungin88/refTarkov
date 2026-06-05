// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Types/RaidTypes.h"
#include "StashSubsystem.generated.h"

UCLASS(Config = Game)
class REFTARKOV_API UStashSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TObjectPtr<class UInventoryContainer> Container;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Stash")
	int32 InitialMaxSlots = 30;

private:
	FRaidStatsSnapshot LastRaidSnapshot;

	bool bHasLastSnapshot = false;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool TryAddItem(const class UItemDataAsset* ItemDef, int32 Quantity, int32& OutRemainder);
	bool RemoveItemAt(int32 SlotIndex, int32 Quantity);
	const TArray<struct FItemInstance>& GetSlots() const;
	UInventoryContainer* GetContainer() const { return Container; }

	void TransferFromInventory(UInventoryContainer* Source);

	bool ExpandSlots(int32 NewMaxSlots);

	void CacheRaidSnapshot(const FRaidStatsSnapshot& Snapshot);
	bool HasLastRaidSnapshot() const { return bHasLastSnapshot; }
	FRaidStatsSnapshot GetLastRaidSnapshot() const { return LastRaidSnapshot; }
};
