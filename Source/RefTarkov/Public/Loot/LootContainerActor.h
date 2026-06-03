// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Loot/Interactable.h"
#include "LootContainerActor.generated.h"

UCLASS()
class REFTARKOV_API ALootContainerActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ALootContainerActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() const override;

	virtual void OnFocusGained_Implementation(AActor* Interactor) override;
	virtual void OnFocusLost_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> PromptWidget;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Container")
	FText DisplayName;

	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	bool bIsFocused = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PromptWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Loot")
	TObjectPtr<class UInventoryContainer> LootContainer;

	UPROPERTY(EditAnywhere, Category = "Loot")
	TSoftObjectPtr<class ULootTableDataAsset> LootTable;

	UPROPERTY(EditDefaultsOnly, Category = "Loot", meta = (ClampMin = "1"))
	int32 ContainerSlotCount = 16;
};
