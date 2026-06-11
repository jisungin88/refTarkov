// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIAlertSubsystem.generated.h"

class AEnemyAIController;

UCLASS()
class REFTARKOV_API UAIAlertSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterEnemy(AEnemyAIController* Controller);
	void UnregisterEnemy(AEnemyAIController* Controller);

	void BroadcastAlert(FVector ThreatLocation, float AlertRadius,
						uint8 SourceTeamId, AEnemyAIController* Instigator);

private:
	TArray<TWeakObjectPtr<AEnemyAIController>> RegisteredEnemies;
};
