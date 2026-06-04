// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/RaidTypes.h"
#include "MinionsGameModeBase.generated.h"

class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRaidEnded,
	APlayerCharacter*, Player, ERaidEndReason, Reason);

UCLASS()
class REFTARKOV_API AMinionsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Raid")
	void NotifyRaidExtracted(APlayerCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Raid")
	void NotifyRaidFailed(APlayerCharacter* Player);

	UFUNCTION(BlueprintPure, Category = "Raid")
	ERaidPhase GetCurrentRaidPhase() const { return CurrentRaidPhase; }

	UPROPERTY(BlueprintAssignable, Category = "Raid")
	FOnRaidEnded OnRaidEnded;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Raid")
	ERaidPhase CurrentRaidPhase = ERaidPhase::InProgress;

private:
	void EndRaid(APlayerCharacter* Player, ERaidEndReason Reason);

	UFUNCTION()
	void HandleRaidEnded_TransferStash(APlayerCharacter* Player, ERaidEndReason Reason);
};
