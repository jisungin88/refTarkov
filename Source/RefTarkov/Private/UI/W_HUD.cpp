// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/W_HUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UW_HUD::UpdateHealth(float Current, float Max)
{
	if (ProgressBar_Health && Max > 0)
	{
		ProgressBar_Health->SetPercent(Current / Max);
	}
}

void UW_HUD::UpdateAmmo(int32 CurrentMag, int32 Reserve)
{
	if (Text_AmmoMag)
	{
		Text_AmmoMag->SetText(FText::AsNumber(CurrentMag));
	}

	if (Text_AmmoReserve)
	{
		Text_AmmoReserve->SetText(FText::AsNumber(Reserve));
	}
}
