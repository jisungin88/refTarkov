// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Strafe.generated.h"

UCLASS()
class REFTARKOV_API UBTService_Strafe : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Strafe();

	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "0.5"))
	float DirectionChangePeriod = 1.5f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	struct FServiceMemory
	{
		float Timer = 0.f;
		float Direction = 1.f;
	};
};
