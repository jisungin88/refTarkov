// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MinionsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class REFTARKOV_API AMinionsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMinionsPlayerController();

protected:
	virtual void BeginPlay() override;
};
