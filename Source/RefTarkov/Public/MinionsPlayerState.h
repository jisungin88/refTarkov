// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MinionsPlayerState.generated.h"

UCLASS()
class REFTARKOV_API AMinionsPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMinionsPlayerState();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raid")
	TObjectPtr<class URaidStatsComponent> RaidStats;
};
