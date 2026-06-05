// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoChangedSignature, int32, CurrentAmmo);

UCLASS()
class REFTARKOV_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MuzzlePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UWeaponDataAsset> WeaponData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmoInMag = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bIsReloading = false;

	float LastFireTime = -FLT_MAX;
	FTimerHandle ReloadTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire(const FVector& TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	virtual void StartReload();

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetCurrentAmmoInMag() const { return CurrentAmmoInMag; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	bool IsReloading() const { return bIsReloading; }

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnAmmoChangedSignature OnAmmoChanged;

protected:
	bool CanFire() const;

	void PerformFireTrace(const FVector& Start, const FVector& End);

	UFUNCTION()
	void FinishReload();
};
