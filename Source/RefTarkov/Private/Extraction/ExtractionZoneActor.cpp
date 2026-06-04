// Fill out your copyright notice in the Description page of Project Settings.


#include "Extraction/ExtractionZoneActor.h"
#include "Components/BoxComponent.h"            
#include "Characters/PlayerCharacter.h"           
#include "MinionsGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AExtractionZoneActor::AExtractionZoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ExtractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Extraction Box"));
	RootComponent = ExtractionBox;
	ExtractionBox->SetBoxExtent(FVector(300.f, 300.f, 100.f));
	ExtractionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void AExtractionZoneActor::BeginPlay()
{
	Super::BeginPlay();
	
	ExtractionBox->OnComponentBeginOverlap.AddDynamic(this, &AExtractionZoneActor::OnExtractionBoxBeginOverlap);
	ExtractionBox->OnComponentEndOverlap.AddDynamic(this, &AExtractionZoneActor::OnExtractionBoxEndOverlap);
}

void AExtractionZoneActor::OnExtractionBoxBeginOverlap(UPrimitiveComponent* Overlapped,
	AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& Sweep)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Other);
	if (!Player)
		return;
	if (ExtractingPlayer.IsValid())
		return;

	ExtractingPlayer = Player;

	UE_LOG(LogTemp, Display, TEXT("[Extraction] %s entered zone - extracting in %.1fs"),
		*Player->GetName(), ExtractionTime);

	GetWorldTimerManager().SetTimer(
		ExtractionTimerHandle, this,
		&AExtractionZoneActor::CompleteExtraction,
		ExtractionTime, false);
}

void AExtractionZoneActor::OnExtractionBoxEndOverlap(UPrimitiveComponent* Overlapped,
	AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Other != ExtractingPlayer.Get())
		return;

	GetWorldTimerManager().ClearTimer(ExtractionTimerHandle);
	ExtractingPlayer.Reset();
	UE_LOG(LogTemp, Warning, TEXT("[Extraction] Player left zone - cancelled"));
}

void AExtractionZoneActor::CompleteExtraction()
{
	if (!ExtractingPlayer.IsValid())
		return;

	if (AMinionsGameModeBase* GM = Cast<AMinionsGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->NotifyRaidExtracted(ExtractingPlayer.Get());
	}

	UE_LOG(LogTemp, Display, TEXT("[Extraction] %s extracted"), *ExtractingPlayer->GetName());
	ExtractingPlayer.Reset();
}