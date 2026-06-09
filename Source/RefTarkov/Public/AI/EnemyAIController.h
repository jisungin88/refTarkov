// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

UCLASS()
class REFTARKOV_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

public:
	static const FName TargetActorKey;
	static const FName InvestigateLocationKey;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBlackboardData> BlackboardAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;

public:
	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* GetSensedTarget() const;

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


public:
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
protected:
	/** 시야 콘 디버그 그리기. WITH_EDITOR에서만. */
	void DrawSightDebug() const;
#endif
};
