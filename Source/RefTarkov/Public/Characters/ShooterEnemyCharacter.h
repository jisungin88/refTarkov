#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacter.h"
#include "ShooterEnemyCharacter.generated.h"

class AWeaponBase;

UCLASS()
class REFTARKOV_API AShooterEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool PerformRangedAttack(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Weapon")
	AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AWeaponBase> CurrentWeapon;
};
