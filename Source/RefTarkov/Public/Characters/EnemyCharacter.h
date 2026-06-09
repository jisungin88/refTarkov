// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MinionCharacterBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyCharacter.generated.h"

class ALootContainerActor;

UCLASS(Abstract)
class REFTARKOV_API AEnemyCharacter : public AMinionCharacterBase
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

public:
	UBehaviorTree* GetBehaviorTree() const { return BehaviorTreeAsset; }

protected:
	virtual void HandleDeath(UHealthComponent* HealthComponent, AController* Killer) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSubclassOf<ALootContainerActor> CorpseClass;
};
