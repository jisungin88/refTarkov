// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LootlingsPlayerState.generated.h"

UCLASS()
class REFTARKOV_API ALootlingsPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ALootlingsPlayerState();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Raid")
	TObjectPtr<class URaidStatsComponent> RaidStats;
};
