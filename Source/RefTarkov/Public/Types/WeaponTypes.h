// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	None,
	Pistol_9mm,
	SMG_45ACP,
	Rifle_556,
	Rifle_762,
	Shotgun_12,
};

UENUM(BlueprintType)
enum class EFireModeType : uint8
{
	Single,
	Burst,
	Auto
};