// Fill out your copyright notice in the Description page of Project Settings.


#include "Loot/LootContainerActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

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
}

void ALootContainerActor::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Display, TEXT("[Loot] %s opened by %s"),
		*GetName(),
		Interactor ? *Interactor->GetName() : TEXT("None"));
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