#include "LootlingsPlayerController.h"
#include "Managers/LootlingsCheatManager.h"
#include "UObject/ConstructorHelpers.h"
#include "LootlingsGameModeBase.h"
#include "LootlingsPlayerState.h"
#include "Components/RaidStatsComponent.h"
#include "UI/W_HUD.h"
#include "UI/W_RaidResult.h"
#include "Characters/PlayerCharacter.h"
#include "Components/HealthComponent.h"
#include "Weapons/WeaponBase.h"

ALootlingsPlayerController::ALootlingsPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	static ConstructorHelpers::FClassFinder<UCheatManager> CheatBPClass(TEXT("/Game/BP_LootlingsCheatManager"));
	if (CheatBPClass.Succeeded())
	{
		CheatClass = CheatBPClass.Class;
	}
	else
	{
		CheatClass = ULootlingsCheatManager::StaticClass();
	}
}

void ALootlingsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드를 Game And UI로 설정하여 게임 조작과 마우스 커서 사용을 동시에 가능하게 합니다.
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UW_HUD>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	ALootlingsGameModeBase* GM =
		GetWorld() ? GetWorld()->GetAuthGameMode<ALootlingsGameModeBase>() : nullptr;
	if (GM)
	{
		GM->OnRaidEnded.AddDynamic(this, &ALootlingsPlayerController::HandleRaidEnded);
	}
}

void ALootlingsPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(InPawn);
	if (!PlayerChar) return;

	if (UHealthComponent* HealthComp = PlayerChar->GetHealthComponent())
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ALootlingsPlayerController::HandleHealthChanged);
		if (HUDWidget)
		{
			HUDWidget->UpdateHealth(HealthComp->GetCurrentHealth(), HealthComp->GetMaxHealth());
		}
	}

	PlayerChar->OnWeaponEquipped.AddDynamic(this, &ALootlingsPlayerController::HandleWeaponEquipped);
}

void ALootlingsPlayerController::HandleRaidEnded(APlayerCharacter* PlayerChar, ERaidEndReason EndReason)
{
	ALootlingsPlayerState* PS = GetPlayerState<ALootlingsPlayerState>();
	if (PS && PS->RaidStats)
	{
		if (RaidResultWidgetClass)
		{
			UW_RaidResult* ResultWidget = CreateWidget<UW_RaidResult>(this, RaidResultWidgetClass);
			if (ResultWidget)
			{
				ResultWidget->InitResult(PS->RaidStats->GetSnapshot());
				ResultWidget->AddToViewport();
			}
		}
	}

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ALootlingsPlayerController::HandleHealthChanged(UHealthComponent* HealthComp,
	float OldHealth, float NewHealth, AController* InstigatedBy)
{
	if (HUDWidget && HealthComp)
	{
		HUDWidget->UpdateHealth(NewHealth, HealthComp->GetMaxHealth());
	}
}

void ALootlingsPlayerController::HandleWeaponEquipped(AWeaponBase* NewWeapon)
{
	if (!NewWeapon || !HUDWidget) return;

	NewWeapon->OnAmmoChanged.AddDynamic(this, &ALootlingsPlayerController::HandleAmmoChanged);

	if (HUDWidget)
	{
		HUDWidget->UpdateAmmo(NewWeapon->GetCurrentAmmoInMag(), 0);
	}
}

void ALootlingsPlayerController::HandleAmmoChanged(int32 CurrentAmmo)
{
	if (HUDWidget)
	{
		HUDWidget->UpdateAmmo(CurrentAmmo, 0);
	}
}
