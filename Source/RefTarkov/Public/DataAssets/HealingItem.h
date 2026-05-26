// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/ConsumableItem.h"
#include "HealingItem.generated.h"

UCLASS(BlueprintType)
class REFTARKOV_API UHealingItem : public UConsumableItem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Healing", meta=(ClampMin="0.0"))
    float HealAmount = 0.f;
};
