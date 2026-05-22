// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MinionCharacterBase.generated.h"

class UHealthComponent;

/**
 * 플레이어와 적이 공유하는 공통 베이스.
 * 인스턴스화 불가(Abstract). BP에서 새 인스턴스 만들면 에디터가 막는다.
 */
UCLASS(Abstract)
class REFTARKOV_API AMinionCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMinionCharacterBase();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComp;

	UFUNCTION()
	virtual void HandleDeath(UHealthComponent* HealthComponent, AController* Killer);
	
public:
	UFUNCTION(BlueprintPure, Category = "Health")
	UHealthComponent* GetHealthComponent() const { return HealthComp; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;

};
