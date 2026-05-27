// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionsPlayerController.h"
#include "Managers/MinionsCheatManager.h"
#include "UObject/ConstructorHelpers.h"

AMinionsPlayerController::AMinionsPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	

	static ConstructorHelpers::FClassFinder<UCheatManager> CheatBPClass(TEXT("/Game/BP_MinionsCheatManager"));
	if (CheatBPClass.Succeeded())
	{
		CheatClass = CheatBPClass.Class;
	}
	else
	{
		CheatClass = UMinionsCheatManager::StaticClass();
	}
}

void AMinionsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드를 Game And UI로 설정하여 게임 조작과 마우스 커서 사용을 동시에 가능하게 합니다.
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

}
