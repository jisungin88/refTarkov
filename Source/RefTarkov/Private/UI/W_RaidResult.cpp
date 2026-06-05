// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_RaidResult.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

void UW_RaidResult::InitResult(const FRaidStatsSnapshot& Stats)
{
	if (Text_ResultTitle)
	{
		const bool bExtracted = Stats.EndReason == ERaidEndReason::Extracted;
		Text_ResultTitle->SetText(FText::FromString(bExtracted ? TEXT("EXTRACTED") : TEXT("KIA")));
	}

	if (Text_KillCount)
	{
		Text_KillCount->SetText(FText::FromString(FString::Printf(TEXT("Kills: %d"), Stats.KillCount)));
	}

	if (Text_SurvivalTime)
	{
		const int32 Min = FMath::Clamp(FMath::RoundToInt(Stats.SurvivalTimeSeconds) / 60, 0, 999);
		const int32 Sec = FMath::Clamp(FMath::RoundToInt(Stats.SurvivalTimeSeconds) % 60, 0, 59);

		Text_SurvivalTime->SetText(FText::FromString(FString::Printf(TEXT("Survival Time: %02d:%02d"), Min, Sec)));
	}

	if (Text_TotalXP)
	{
		Text_TotalXP->SetText(FText::FromString(FString::Printf(TEXT("XP: +%d"), Stats.TotalXP)));
	}

	if (VerticalBox_Items)
	{
		VerticalBox_Items->ClearChildren();
		for (const FItemInstance& Item : Stats.LootedItems)
		{
			if (!Item.ItemDef)
				continue;

			UTextBlock* ItemText = NewObject<UTextBlock>(VerticalBox_Items);
			ItemText->SetText(FText::FromString(FString::Printf(TEXT("%s x%d"), *Item.ItemDef->DisplayName.ToString(), Item.StackCount)));
			VerticalBox_Items->AddChild(ItemText);
		}
	}
}

void UW_RaidResult::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Confirm)
	{
		Button_Confirm->OnClicked.AddDynamic(this, &UW_RaidResult::OnConfirmClicked);
	}
}

void UW_RaidResult::OnConfirmClicked()
{
	UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));
}
