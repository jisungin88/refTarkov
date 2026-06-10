// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

UCLASS()
class REFTARKOV_API UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MeleeAttack();

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "1.0"))
	float RotationSpeed = 360.f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	struct FTaskMemory
	{
		FRotator TargetRotation = FRotator::ZeroRotator;
	};


};
