// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShootAtPlayer.generated.h"

/**
 * 
 */
UCLASS()
class REFTARKOV_API UBTTask_ShootAtPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ShootAtPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;
};
