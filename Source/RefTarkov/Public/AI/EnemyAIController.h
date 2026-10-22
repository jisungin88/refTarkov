#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UHealthComponent;

UCLASS()
class REFTARKOV_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	static const FName TargetActorKey;
	static const FName SuspicionTargetKey;
	static const FName InvestigateLocationKey;
	static const FName HomeLocationKey;
	static const FName ShouldReturnHomeKey;
	static const FName CombatRoleKey;
	static const FName CoverWeightKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Perception", meta = (ClampMin = "0.1", ToolTip = "시야 인지 후 전투 확정까지 걸리는 초"))
	float SuspicionDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Leash", meta = (ClampMin = "100.0"))
	float LeashRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Alert", meta = (ClampMin = "100.0"))
	float AlertBroadcastRadius = 1500.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<class UBlackboardData> BlackboardAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI|LOD", meta = (ClampMin = "100.0"))
	float LODNearRange = 1500.f;    // 이 거리 이내: 풀 틱

	UPROPERTY(EditDefaultsOnly, Category = "AI|LOD", meta = (ClampMin = "100.0"))
	float LODFarRange = 3500.f;     // 이 거리 이상: 저빈도 틱

	/** 체력 0%일 때 엄폐 점수 가중치 (100%일 때 1.0 */
	UPROPERTY(EditDefaultsOnly, Category = "AI|Cover", meta = (ClampMin = "1.0"))
	float MaxCoverWeight = 3.f;

public:
	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* GetSensedTarget() const;

	void EnterCombat(AActor* Target);
	void ExitCombat();

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleHealthChanged(UHealthComponent* HealthComp, float OldHealth, float NewHealth, AController* InstigatedBy);

	void UpdateCoverWeight(const UHealthComponent* HealthComp);

	void UpdateLeash();

	TWeakObjectPtr<UHealthComponent> BoundHealthComp;

public:
	virtual void Tick(float DeltaTime) override;
	void UpdateLOD();

#if ENABLE_DRAW_DEBUG
protected:
	/** 시야 콘 디버그 그리기. WITH_EDITOR에서만. */
	void DrawSightDebug() const;
#endif
};
