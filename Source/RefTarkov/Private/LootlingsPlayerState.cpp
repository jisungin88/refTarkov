// Fill out your copyright notice in the Description page of Project Settings.


#include "LootlingsPlayerState.h"
#include "Components/RaidStatsComponent.h"

ALootlingsPlayerState::ALootlingsPlayerState()
{
    RaidStats = CreateDefaultSubobject<URaidStatsComponent>(TEXT("RaidStats"));
}