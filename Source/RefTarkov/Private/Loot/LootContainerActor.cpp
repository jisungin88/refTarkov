// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/LootContainerActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/Items/ItemInstance.h"
#include "Inventory/DataAssets/ItemDataAsset.h"
#include "Loot/LootTableDataAsset.h"

ALootContainerActor::ALootContainerActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Prompt Widget"));
	PromptWidget->SetupAttachment(RootComponent);
	PromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	PromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PromptWidget->SetDrawAtDesiredSize(true);
	PromptWidget->SetVisibility(false);
}

void ALootContainerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsFocused)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(),
			FVector(60.f, 60.f, 60.f),
			FColor::Yellow, false, -1.f, 0, 3.f);
	}
}

void ALootContainerActor::BeginPlay()
{
	Super::BeginPlay();
	if (PromptWidget && PromptWidgetClass)
	{
		PromptWidget->SetWidgetClass(PromptWidgetClass);
	}

	LootContainer = NewObject<UInventoryContainer>(this);
	LootContainer->Initialize(ContainerSlotCount, -1.f);

	if (ULootTableDataAsset* Table = LootTable.LoadSynchronous())
	{
		Table->RollAndAddTo(LootContainer);
	}
}

void ALootContainerActor::Interact_Implementation(AActor* Interactor)
{
	if (!LootContainer || !Interactor)
		return;

	UInventoryComponent* PlayerInv = Interactor->FindComponentByClass<UInventoryComponent>();
	if (!PlayerInv)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interactor %s does not have an inventory component"), *Interactor->GetName());
		return;
	}

	const TArray<FItemInstance>& ContainerItems = LootContainer->GetSlots();
	for (int32 i = 0; i < ContainerItems.Num(); ++i)
	{
		const FItemInstance& Item = ContainerItems[i];
		if (Item.IsEmpty())
			continue;

		UE_LOG(LogTemp, Display, TEXT("Attempting to loot %d x %s"), Item.StackCount, *Item.GetItemDef()->DisplayName.ToString());
		int32 Remainder = 0;
		PlayerInv->TryAddItem(Item.GetItemDef(), Item.StackCount, Remainder);
		
		const int32 RemovedCount = Item.StackCount - Remainder;
		if (RemovedCount > 0)
		{
			UE_LOG(LogTemp, Display, TEXT("Looted %d x %s"), RemovedCount, *Item.GetItemDef()->DisplayName.ToString());
			LootContainer->RemoveItemAt(i, RemovedCount);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("[Loot] %s looted by %s"), *GetName(), *Interactor->GetName());
}

FText ALootContainerActor::GetInteractPrompt_Implementation() const
{
	return DisplayName.IsEmpty() ?
		FText::FromString(TEXT("Open Container")) : DisplayName;
}

void ALootContainerActor::OnFocusGained_Implementation(AActor* Interactor)
{
	bIsFocused = true;
	if (PromptWidget)
	{
		PromptWidget->SetVisibility(true);
	}
}

void ALootContainerActor::OnFocusLost_Implementation(AActor* Interactor)
{
	bIsFocused = false;
	if (PromptWidget)
	{
		PromptWidget->SetVisibility(false);
	}
}