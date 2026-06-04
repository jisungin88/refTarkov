// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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