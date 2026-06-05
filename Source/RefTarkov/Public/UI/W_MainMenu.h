// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/RaidTypes.h"
#include "W_MainMenu.generated.h"

class UTextBlock;
class UButton;
class UWidget;

UCLASS()
class REFTARKOV_API UW_MainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_StartRaid;;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Quit;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> Widget_LastStats;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_LastResult;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_LastKills;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_LastXP;

private:
	void PopulateLastStats(const FRaidStatsSnapshot& Snapshot);

	UFUNCTION()
	void OnStartRaidClicked();

	UFUNCTION()
	void OnQuitClicked();
};
