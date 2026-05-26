// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/ItemDataAsset.h"
#include "DebugComponent.generated.h"

UCLASS( ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class REFTARKOV_API UDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDebugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(Exec, Category = "Debug")
	void DebugAddItem(FName Name, int32 Quantity);

	UFUNCTION(Exec, Category = "Debug")
	void DebugRemoveItemAt(int32 SlotIndex, int32 Quantity);

	UFUNCTION(Exec, Category = "Debug")
	void DebugDumpInventory();

protected:
	UPROPERTY(EditAnywhere, Category = "Debug")
	TMap<FName, TSoftObjectPtr<UItemDataAsset>> DebugItemTable;
};
