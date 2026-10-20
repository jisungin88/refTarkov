// Fill out your copyright notice in the Description page of Project Settings.
#include "LootlingsGameModeBase.h"
#include "Characters/PlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/StashSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "LootlingsPlayerState.h"
#include "Components/RaidStatsComponent.h"

ALootlingsGameModeBase::ALootlingsGameModeBase()
{
	PlayerStateClass = ALootlingsPlayerState::StaticClass();
}

void ALootlingsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FGenericTeamId::SetAttitudeSolver([](FGenericTeamId A, FGenericTeamId B)
	{
		if (A == B)
			return ETeamAttitude::Friendly;
		else if (A == 254 || B == 254)
			return ETeamAttitude::Neutral;
		else
			return ETeamAttitude::Hostile;
	});

	
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ALootlingsPlayerState* PS = PC->GetPlayerState<ALootlingsPlayerState>())
		{
			PS->RaidStats->StartRaid();
		}
	}

    OnRaidEnded.AddDynamic(this, &ALootlingsGameModeBase::HandleRaidEnded_TransferStash);
	OnRaidEnded.AddDynamic(this, &ALootlingsGameModeBase::HandleRaidEnded_CacheStats);
}

void ALootlingsGameModeBase::NotifyRaidExtracted(APlayerCharacter* Player)
{
	EndRaid(Player, ERaidEndReason::Extracted);
}

void ALootlingsGameModeBase::NotifyRaidFailed(APlayerCharacter* Player)
{
	EndRaid(Player, ERaidEndReason::Killed);
}

void ALootlingsGameModeBase::EndRaid(APlayerCharacter* Player, ERaidEndReason Reason)
{
	if (CurrentRaidPhase == ERaidPhase::Ended)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Raid] EndRaid called but already Ended (Reason=%s ignored)"),
			*StaticEnum<ERaidEndReason>()->GetValueAsString(Reason));
		return;
	}

	CurrentRaidPhase = ERaidPhase::Ended;

	UE_LOG(LogTemp, Display, TEXT("=== RAID ENDED: %s ==="),
		*StaticEnum<ERaidEndReason>()->GetValueAsString(Reason));

	if (Reason == ERaidEndReason::Extracted && Player)
	{
		if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
		{
			Player->DisableInput(PC);
		}
	}

	if (Player)
	{
		if (ALootlingsPlayerState* PS = Player->GetPlayerState<ALootlingsPlayerState>())
		{
			PS->RaidStats->EndRaid(Reason);
		}
	}

	OnRaidEnded.Broadcast(Player, Reason);
}

void ALootlingsGameModeBase::HandleRaidEnded_TransferStash(APlayerCharacter* Player, ERaidEndReason Reason)
{
	if (Reason != ERaidEndReason::Extracted)
		return;
	if (!Player)
		return;

	UInventoryComponent* Inv = Player->FindComponentByClass<UInventoryComponent>();
	if (!Inv)
		return;

	UInventoryContainer* Container = Inv->GetContainer();
	if (!Container)
		return;

	UStashSubsystem* Stash = GetGameInstance()->GetSubsystem<UStashSubsystem>();
	if (!Stash)
		return;

	Stash->TransferFromInventory(Container);
	UE_LOG(LogTemp, Display, TEXT("[Raid] Stash transfer complete"));
}

void ALootlingsGameModeBase::HandleRaidEnded_CacheStats(APlayerCharacter* Player, ERaidEndReason Reason)
{
	if (!Player) return;

	ALootlingsPlayerState* PS = Player->GetPlayerState<ALootlingsPlayerState>();
	if (!PS) return;

	UStashSubsystem* Stash = GetGameInstance()->GetSubsystem<UStashSubsystem>();
	if (!Stash) return;

	Stash->CacheRaidSnapshot(PS->RaidStats->GetSnapshot());
}
