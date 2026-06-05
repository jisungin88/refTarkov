// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_HUD.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class REFTARKOV_API UW_HUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealth(float Current, float Max);
	void UpdateAmmo(int32 CurrentMag, int32 Reserve);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AmmoMag;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AmmoReserve;
};
