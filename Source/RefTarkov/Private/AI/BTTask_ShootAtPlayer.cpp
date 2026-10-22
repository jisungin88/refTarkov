#include "AI/BTTask_ShootAtPlayer.h"
#include "AIController.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/ShooterEnemyCharacter.h"
#include "Weapons/WeaponBase.h"

UBTTask_ShootAtPlayer::UBTTask_ShootAtPlayer()
{
	NodeName = TEXT("Shoot At Player");
	bNotifyTick = true;

	TargetKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTTask_ShootAtPlayer, TargetKey),
		AActor::StaticClass());

	TargetKey.SelectedKeyName = AEnemyAIController::TargetActorKey;
}

void UBTTask_ShootAtPlayer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (UBlackboardData* BBData = GetBlackboardAsset())
	{
		TargetKey.ResolveSelectedKey(*BBData);
	}
}

EBTNodeResult::Type UBTTask_ShootAtPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIC = OwnerComp.GetAIOwner();
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AIC || !BB || !Cast<AShooterEnemyCharacter>(AIC->GetPawn()))
	{
		return EBTNodeResult::Failed;
	}

	if (!BB->GetValueAsObject(TargetKey.SelectedKeyName))
	{
		return EBTNodeResult::Failed;
	}

	FShootMemory* Memory = reinterpret_cast<FShootMemory*>(NodeMemory);
	Memory->ShotsRemaining = FMath::RandRange(MinBurstCount, FMath::Max(MinBurstCount, MaxBurstCount));
	return EBTNodeResult::InProgress;
}

void UBTTask_ShootAtPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float dt)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AShooterEnemyCharacter* Shooter = AIC ? Cast<AShooterEnemyCharacter>(AIC->GetPawn()) : nullptr;
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName)) : nullptr;
	if (!Shooter || !Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 사선이 막히면 종료
	if (!AIC->LineOfSightTo(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 재장전일 때 버스트 유지
	const AWeaponBase* Weapon = Shooter->GetCurrentWeapon();
	if (Weapon && Weapon->IsReloading())
	{
		return;
	}

	// 포커스로  돌아오는 중일 때
	const FVector ToTarget = (Target->GetActorLocation() - Shooter->GetActorLocation()).GetSafeNormal2D();
	const float FacingDot = FVector::DotProduct(Shooter->GetActorForwardVector().GetSafeNormal2D(), ToTarget);
	if (FacingDot < FMath::Cos(FMath::DegreesToRadians(AimToleranceDegrees)))
	{
		return;
	}

	// RPM 게이팅에 막힐 때
	if (Shooter->PerformRangedAttack(Target))
	{
		FShootMemory* Memory = reinterpret_cast<FShootMemory*>(NodeMemory);
		if (--Memory->ShotsRemaining <= 0)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}

uint16 UBTTask_ShootAtPlayer::GetInstanceMemorySize() const
{
	return sizeof(FShootMemory);
}
