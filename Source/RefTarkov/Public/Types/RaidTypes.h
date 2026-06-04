// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/ItemInstance.h"
#include "RaidTypes.generated.h"

UENUM(BlueprintType)
enum class ERaidPhase : uint8
{
	WaitingToStart,
	InProgress,
	Ended
};

UENUM(BlueprintType)
enum class ERaidEndReason : uint8
{
	None,
	Extracted,
	Killed,
	TimeUp,
	Aborted
};

USTRUCT(BlueprintType)
struct FRaidStatsSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) 
	int32 KillCount = 0;

	UPROPERTY(BlueprintReadOnly) 
	float SurvivalTimeSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly) 
	TArray<FItemInstance> LootedItems;

	UPROPERTY(BlueprintReadOnly) 
	int32 TotalXP = 0;

	UPROPERTY(BlueprintReadOnly) 
	ERaidEndReason EndReason = ERaidEndReason::None;
};