// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class REFTARKOV_API IInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category="Interaction")
    void Interact(AActor* Interactor);
    virtual void Interact_Implementation(AActor* Interactor) {}

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    FText GetInteractPrompt() const;
    virtual FText GetInteractPrompt_Implementation() const { return FText::FromString(TEXT("Interact")); }

    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    void OnFocusGained(AActor* Interactor);
    virtual void OnFocusGained_Implementation(AActor* Interactor) {}

    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    void OnFocusLost(AActor* Interactor);
    virtual void OnFocusLost_Implementation(AActor* Interactor) {}
};