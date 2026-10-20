// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "LootlingCharacterBase.generated.h"

class UHealthComponent;

/**
 * 플레이어와 적이 공유하는 공통 베이스.
 * 인스턴스화 불가(Abstract). BP에서 새 인스턴스 만들면 에디터가 막는다.
 */
UCLASS(Abstract)
class REFTARKOV_API ALootlingCharacterBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ALootlingCharacterBase();

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team", meta = (ClampMin = "0", ClampMax = "254"))
	uint8 TeamId = 1;  // 0은 플레이어, 254는 "모든 팀". 기본값은 1 (적).
	
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
