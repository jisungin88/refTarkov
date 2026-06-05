// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MenuPlayerController.h"
#include "UI/W_MainMenu.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (MainMenuWidgetClass)
	{
		UW_MainMenu* MainMenuWidget = CreateWidget<UW_MainMenu>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(MainMenuWidget->TakeWidget());
			SetInputMode(InputModeData);
			bShowMouseCursor = true;
		}
	}
}
