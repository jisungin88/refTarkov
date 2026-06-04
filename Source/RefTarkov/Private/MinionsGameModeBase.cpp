// Fill out your copyright notice in the Description page of Project Settings.
#include "MinionsGameModeBase.h"
#include "Characters/PlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/StashSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "MinionsPlayerState.h"

AMinionsGameModeBase::AMinionsGameModeBase()
{
	PlayerStateClass = AMinionsPlayerState::StaticClass();
}

void AMinionsGameModeBase::BeginPlay()
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
		if (AMinionsPlayerState* PS = PC->GetPlayerState<AMinionsPlayerState>())
		{
			PS->RaidStats->StartRaid();
		}
	}

    OnRaidEnded.AddDynamic(this, &AMinionsGameModeBase::HandleRaidEnded_TransferStash);
}

void AMinionsGameModeBase::NotifyRaidExtracted(APlayerCharacter* Player)
{
	EndRaid(Player, ERaidEndReason::Extracted);
}

void AMinionsGameModeBase::NotifyRaidFailed(APlayerCharacter* Player)
{
	EndRaid(Player, ERaidEndReason::Killed);
}

void AMinionsGameModeBase::EndRaid(APlayerCharacter* Player, ERaidEndReason Reason)
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
		if (AMinionsPlayerState* PS = Player->GetPlayerState<AMinionsPlayerState>())
		{
			PS->RaidStats->EndRaid(Reason);
		}
	}

	OnRaidEnded.Broadcast(Player, Reason);
}

void AMinionsGameModeBase::HandleRaidEnded_TransferStash(APlayerCharacter* Player, ERaidEndReason Reason)
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