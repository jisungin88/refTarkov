// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Types/RaidTypes.h"
#include "MinionsPlayerController.generated.h"

class UHealthComponent;
class AWeaponBase;
class APlayerCharacter;

UCLASS()
class REFTARKOV_API AMinionsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMinionsPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UW_HUD> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UW_RaidResult> RaidResultWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UW_HUD> HUDWidget;

	UFUNCTION()
	void HandleRaidEnded(APlayerCharacter* PlayerChar, ERaidEndReason EndReason);

	UFUNCTION()
	void HandleHealthChanged(UHealthComponent* HealthComp,
		float OldHealth, float NewHealth, AController* InstigatedBy);

	UFUNCTION()
	void HandleWeaponEquipped(AWeaponBase* NewWeapon);

	UFUNCTION()
	void HandleAmmoChanged(int32 CurrentAmmo);
};
