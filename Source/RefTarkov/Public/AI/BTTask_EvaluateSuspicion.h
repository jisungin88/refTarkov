// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_EvaluateSuspicion.generated.h"

UCLASS()
class REFTARKOV_API UBTTask_EvaluateSuspicion : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_EvaluateSuspicion();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SuspicionTargetKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	struct FTaskMemory { float ElapsedTime = 0.f; };
};
