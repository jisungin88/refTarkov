// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/RaidTypes.h"
#include "W_RaidResult.generated.h"

class UTextBlock;
class UButton;
class UVerticalBox;

UCLASS()
class REFTARKOV_API UW_RaidResult : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitResult(const FRaidStatsSnapshot& Stats);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ResultTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_KillCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SurvivalTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TotalXP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Items;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Confirm;

private:
	UFUNCTION()
	void OnConfirmClicked();
};
