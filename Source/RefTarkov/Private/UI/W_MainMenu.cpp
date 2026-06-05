// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_MainMenu.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Inventory/StashSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_StartRaid)
	{
		Btn_StartRaid->OnClicked.AddDynamic(this, &UW_MainMenu::OnStartRaidClicked);
	}

	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.AddDynamic(this, &UW_MainMenu::OnQuitClicked);
	}

	UStashSubsystem* Stash = GetGameInstance()->GetSubsystem<UStashSubsystem>();
	if (Stash && Stash->HasLastRaidSnapshot())
	{
		PopulateLastStats(Stash->GetLastRaidSnapshot());
		if (Widget_LastStats)
		{
			Widget_LastStats->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		if (Widget_LastStats)
		{
			Widget_LastStats->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UW_MainMenu::PopulateLastStats(const FRaidStatsSnapshot& Snapshot)
{
	if (Txt_LastResult)
	{
		const bool bExtracted = Snapshot.EndReason == ERaidEndReason::Extracted;
		Txt_LastResult->SetText(FText::FromString(
			bExtracted ? TEXT("Last Raid: EXTRACTED") : TEXT("Last Raid: KIA")));
	}

	if (Txt_LastKills)
	{
		Txt_LastKills->SetText(FText::FromString(FString::Printf(TEXT("Kills: %d"), Snapshot.KillCount)));
	}

	if (Txt_LastXP)
	{
		Txt_LastXP->SetText(FText::FromString(FString::Printf(TEXT("XP Gained: +%d"), Snapshot.TotalXP)));
	}
}

void UW_MainMenu::OnStartRaidClicked()
{
	UGameplayStatics::OpenLevel(this, TEXT("World"));
}

void UW_MainMenu::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
