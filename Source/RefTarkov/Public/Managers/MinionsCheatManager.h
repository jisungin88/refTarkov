// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "MinionsCheatManager.generated.h"

UCLASS()
class REFTARKOV_API UMinionsCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec, Category = "Debug")
	void DebugAddItem(FName Name, int32 Quantity);

	UFUNCTION(Exec, Category = "Debug")
	void DebugRemoveItemAt(int32 SlotIndex, int32 Quantity);

	UFUNCTION(Exec, Category = "Debug")
	void DebugDumpInventory();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TMap<FName, TSoftObjectPtr<class UItemDataAsset>> DebugItemTable;

private:
	class UInventoryComponent* GetTargetInventory() const;
};
