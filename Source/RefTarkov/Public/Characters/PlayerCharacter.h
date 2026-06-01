// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MinionCharacterBase.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputAction;

UCLASS()
class REFTARKOV_API APlayerCharacter : public AMinionCharacterBase
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInventoryComponent> InventoryComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<class USphereComponent> InteractionSphere;

	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	TArray<TObjectPtr<AActor>> OverlappingInteractables;

	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	TWeakObjectPtr<AActor> CurrentFocusedInteractable;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;

	FVector CachedMouseGroundLocation = FVector::ZeroVector;

protected:
	void Move(const FInputActionValue& Value);
	void OnFireInput();
	void OnReloadInput();

protected:
	UFUNCTION()
	void OnInteractSphereBeginOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnInteractSphereEndOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void OnInteract();
	AActor* GetBestInteractable() const;

protected:
	virtual void HandleDeath(class UHealthComponent* HealthComp, AController* Killer) override;

private:
	void UpdateAimFromMouse();

	void UpdateInteractableFocus();
};
