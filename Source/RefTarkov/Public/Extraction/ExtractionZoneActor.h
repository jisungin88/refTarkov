// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExtractionZoneActor.generated.h"

UCLASS()
class REFTARKOV_API AExtractionZoneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AExtractionZoneActor();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> ExtractionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Extraction", meta = (ClampMin = "0.1"))
	float ExtractionTime = 5.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Extraction")
	TWeakObjectPtr<class APlayerCharacter> ExtractingPlayer;

	FTimerHandle ExtractionTimerHandle;

protected:
	UFUNCTION()
	void OnExtractionBoxBeginOverlap(UPrimitiveComponent* Overlapped,
		AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnExtractionBoxEndOverlap(UPrimitiveComponent* Overlapped,
		AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CompleteExtraction();
};
