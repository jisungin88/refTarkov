// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/RaidTypes.h"
#include "Inventory/Items/ItemInstance.h"
#include "RaidStatsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REFTARKOV_API URaidStatsComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	int32 KillCount = 0;
	float RaidStartTime = 0.f;
	TArray<FItemInstance> LootedItems;

	FRaidStatsSnapshot CachedSnapshot;

	static int32 CalcXP(const FRaidStatsSnapshot& Snap);

public:
	void StartRaid();
	void EndRaid(ERaidEndReason Reason);

	void RecordKill();
	void RecordLoot(const FItemInstance& Item);

	UFUNCTION(BlueprintPure)
	FRaidStatsSnapshot GetSnapshot() const { return CachedSnapshot; }
};
