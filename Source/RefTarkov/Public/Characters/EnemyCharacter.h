// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MinionCharacterBase.h"
#include "Loot/Interactable.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyCharacter.generated.h"

class USphereComponent;
class UInventoryContainer;
class ULootTableDataAsset;

UCLASS(Abstract)
class REFTARKOV_API AEnemyCharacter : public AMinionCharacterBase, public IInteractable
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

public:
	UBehaviorTree* GetBehaviorTree() const { return BehaviorTreeAsset; }

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void HandleDeath(UHealthComponent* HealthComponent, AController* Killer) override;

protected:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() const override;
	virtual void OnFocusGained_Implementation(AActor* Interactor) override;
	virtual void OnFocusLost_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintreadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot")
	TObjectPtr<USphereComponent> CorpseDetectionSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSoftObjectPtr<ULootTableDataAsset> LootTable;

	UPROPERTY(EditDefaultsOnly, Category = "Loot", meta = (ClampMin = "1"))
	int32 CorpseSlotCount = 8;

private:
	UPROPERTY()
	TObjectPtr<UInventoryContainer> CorpseLoot;

	bool bAlreadyLooted = false;

	void InitCorpseLoot();
};
