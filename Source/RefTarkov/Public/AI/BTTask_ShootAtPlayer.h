#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShootAtPlayer.generated.h"

UCLASS()
class REFTARKOV_API UBTTask_ShootAtPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ShootAtPlayer();

protected:
	//~ UBTTaskNode interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float dt) override;
	virtual uint16 GetInstanceMemorySize() const override;
	//~ End UBTTaskNode interface

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Shoot", meta = (ClampMin = "1"))
	int32 MinBurstCount = 2;

	UPROPERTY(EditAnywhere, Category = "Shoot", meta = (ClampMin = "1"))
	int32 MaxBurstCount = 5;

	/** 바라보는 방향과 타겟 방향의 각도 차이 제한 */
	UPROPERTY(EditAnywhere, Category = "Shoot", meta = (ClampMin = "0.0", ClampMax = "90.0"))
	float AimToleranceDegrees = 15.f;

private:
	struct FShootMemory { int32 ShotsRemaining = 0; };
};
