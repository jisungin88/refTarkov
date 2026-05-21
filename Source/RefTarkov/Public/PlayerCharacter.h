// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputAction;

UCLASS()
class REFTARKOV_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 카메라 세팅
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> TopDownCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float WeaponDamage = 35;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float WeaponRange = 1000;
protected:
	void Move(const FInputActionValue& Value);

protected:
	/*UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Effects")
	void PlayShootEffects(FVector TraceStart, FVector TraceEnd, bool bHit, FVector HitLocation);*/

	void Shoot();

private:
	// 마우스 방향을 바라보도록.
	void LookAtMouseCursor();
};
