// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionsPlayerState.h"
#include "Components/RaidStatsComponent.h"

AMinionsPlayerState::AMinionsPlayerState()
{
    RaidStats = CreateDefaultSubobject<URaidStatsComponent>(TEXT("RaidStats"));
}