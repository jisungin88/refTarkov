#include "AI/EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Characters/EnemyCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "AI/AIAlertSubsystem.h"
#include "AI/CombatDirectorSubsystem.h"
#include "Components/HealthComponent.h"
#include "DrawDebugHelpers.h"

const FName AEnemyAIController::TargetActorKey(TEXT("TargetActor"));
const FName AEnemyAIController::SuspicionTargetKey(TEXT("SuspicionTarget"));
const FName AEnemyAIController::InvestigateLocationKey(TEXT("InvestigateLocation"));
const FName AEnemyAIController::HomeLocationKey(TEXT("HomeLocation"));
const FName AEnemyAIController::ShouldReturnHomeKey(TEXT("ShouldReturnHome"));
const FName AEnemyAIController::CombatRoleKey(TEXT("CombatRole"));
const FName AEnemyAIController::CoverWeightKey(TEXT("CoverWeight"));

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;  // Tick 켜기 (디버그용)

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1500.f;                       // 시야 거리
	SightConfig->LoseSightRadius = 2000.f;                   // 이 거리 밖으로 가면 시야에서 잃음
	SightConfig->PeripheralVisionAngleDegrees = 70.f;        // 좌우 각도(총 시야각 180도)
	SightConfig->SetMaxAge(5.f);                             // 5초 동안은 잃어도 stim 유지
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 0.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 1200.f;
	HearingConfig->SetMaxAge(3.f);
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->ConfigureSense(*HearingConfig);

	AIPerception->SetDominantSense(UAISense_Sight::StaticClass());
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,
			&AEnemyAIController::HandleTargetPerceptionUpdated);
	}

	if (UAIAlertSubsystem* AlertSys = GetWorld()->GetSubsystem<UAIAlertSubsystem>())
	{
		AlertSys->RegisterEnemy(this);
	}
}

void AEnemyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAIAlertSubsystem* AlertSys = GetWorld()->GetSubsystem<UAIAlertSubsystem>())
	{
		AlertSys->UnregisterEnemy(this);
	}
	Super::EndPlay(EndPlayReason);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		SetGenericTeamId(TeamAgent->GetGenericTeamId());
	}

	if (BlackboardAsset)
	{
		UBlackboardComponent* BBComp = nullptr;
		UseBlackboard(BlackboardAsset, BBComp);
		if (BBComp)
		{
			BBComp->SetValueAsVector(HomeLocationKey, InPawn->GetActorLocation());
		}
	}

	// BT 시작
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn))
	{
		if (UBehaviorTree* BT = Enemy->GetBehaviorTree())
		{
			RunBehaviorTree(BT);
		}
	}

	if (UHealthComponent* HealthComp = InPawn->FindComponentByClass<UHealthComponent>())
	{
		BoundHealthComp = HealthComp;
		HealthComp->OnHealthChanged.AddDynamic(this, &AEnemyAIController::HandleHealthChanged);
		UpdateCoverWeight(HealthComp);
	}
}

void AEnemyAIController::OnUnPossess()
{
	if (UHealthComponent* HealthComp = BoundHealthComp.Get())
	{
		HealthComp->OnHealthChanged.RemoveDynamic(this, &AEnemyAIController::HandleHealthChanged);
	}
	BoundHealthComp.Reset();

	Super::OnUnPossess();
}

void AEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !Blackboard)
		return;

	const bool bSight = Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>();
	const bool bHearing = Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>();

	if (Stimulus.WasSuccessfullySensed())
	{
		if (bSight)
		{
			if (Blackboard->GetValueAsObject(TargetActorKey))
				return;
			if (Blackboard->GetValueAsObject(SuspicionTargetKey))
				return;

			Blackboard->SetValueAsObject(SuspicionTargetKey, Actor);

			if (UAIAlertSubsystem* AlertSys = GetWorld()->GetSubsystem<UAIAlertSubsystem>())
			{
				AlertSys->BroadcastAlert(Actor->GetActorLocation(), AlertBroadcastRadius,
										GetGenericTeamId().GetId(), this);
			}
		}
		else if (bHearing)
		{
			// 이미 타겟을 보고 있으면 조사 상태로 격하하지 않음
			if (!Blackboard->GetValueAsObject(TargetActorKey))
			{
				Blackboard->SetValueAsVector(InvestigateLocationKey, Stimulus.StimulusLocation);
			}
		}
	}
	else
	{
		if (bSight)
		{
			Blackboard->SetValueAsVector(InvestigateLocationKey, Stimulus.StimulusLocation);
			ExitCombat();
		}
		else if (bHearing)
			Blackboard->ClearValue(InvestigateLocationKey);
	}
}

void AEnemyAIController::HandleHealthChanged(UHealthComponent* HealthComp, float OldHealth, float NewHealth, AController* InstigatedBy)
{
	UpdateCoverWeight(HealthComp);
}

void AEnemyAIController::UpdateCoverWeight(const UHealthComponent* HealthComp)
{
	if (!Blackboard || !HealthComp)
	{
		return;
	}

	// 스폰 직후엔 HealthComponent::BeginPlay 전이라 CurrentHealth=0 → 만피로 취급, BeginPlay 브로드캐스트가 바로 덮어쓴다
	const float HealthPercent = HealthComp->GetCurrentHealth() > 0.f ? HealthComp->GetHealthPercent() : 1.f;
	Blackboard->SetValueAsFloat(CoverWeightKey, FMath::Lerp(1.f, MaxCoverWeight, 1.f - HealthPercent));
}

void AEnemyAIController::UpdateLeash()
{
	if (!Blackboard || !GetPawn())
	{
		return;
	}

	const FVector Home = Blackboard->GetValueAsVector(HomeLocationKey);
	const float DistSq = FVector::DistSquared(GetPawn()->GetActorLocation(), Home);
	const bool bTooFar = DistSq > FMath::Square(LeashRadius);

	const bool bInThreatState = Blackboard->GetValueAsObject(TargetActorKey) != nullptr
								|| Blackboard->GetValueAsObject(SuspicionTargetKey) != nullptr;

	if (bTooFar && bInThreatState)
	{
		Blackboard->SetValueAsBool(ShouldReturnHomeKey, true);
	}
	else
	{
		Blackboard->ClearValue(ShouldReturnHomeKey);
	}
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateLOD();
	UpdateLeash();

#if ENABLE_DRAW_DEBUG
	DrawSightDebug();
#endif
}

#if ENABLE_DRAW_DEBUG
void AEnemyAIController::DrawSightDebug() const
{
	if (!SightConfig || !GetPawn()) return;

	const FVector EyeLocation = GetPawn()->GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FVector ForwardDir = GetPawn()->GetActorForwardVector();

	// 시야각: PeripheralVisionAngleDegrees는 "중심으로부터 좌우 각도".
	// DrawDebugCone은 라디안 단위의 절반각 받음 → 그대로 변환.
	const float HalfAngleRad = FMath::DegreesToRadians(SightConfig->PeripheralVisionAngleDegrees);

	// 인지 거리(노란) — SightRadius
	DrawDebugCone(GetWorld(), EyeLocation, ForwardDir,
		SightConfig->SightRadius,
		HalfAngleRad, HalfAngleRad,
		16, FColor::Yellow, false, -1.f, 0, 1.5f);

	// 잃기 거리(빨강) — LoseSightRadius. 시야각 무관, 단순 거리.
	DrawDebugCircle(GetWorld(), EyeLocation,
		SightConfig->LoseSightRadius, 48,
		FColor::Red, false, -1.f, 0, 1.f,
		FVector(0, 1, 0), FVector(1, 0, 0), false);
}
#endif

AActor* AEnemyAIController::GetSensedTarget() const
{
	if (Blackboard)
	{
		return Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey));
	}
	return nullptr;
}

void AEnemyAIController::EnterCombat(AActor* Target)
{
	if (!Blackboard || !Target)
		return;
	Blackboard->SetValueAsObject(TargetActorKey, Target);

	if (UCombatDirectorSubsystem* Director = GetWorld()->GetSubsystem<UCombatDirectorSubsystem>())
		Director->RegisterCombatant(this);
}

void AEnemyAIController::ExitCombat()
{
	if (Blackboard)
	{
		Blackboard->ClearValue(TargetActorKey);
		Blackboard->ClearValue(SuspicionTargetKey);
	}

	if (UCombatDirectorSubsystem* Director = GetWorld()->GetSubsystem<UCombatDirectorSubsystem>())
	{
		Director->UnregisterCombatant(this);
	}
}

void AEnemyAIController::UpdateLOD()
{
	if (!GetPawn()) return;

	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->GetPawn()) return;

	const float DistSq = FVector::DistSquared(
		GetPawn()->GetActorLocation(),
		PC->GetPawn()->GetActorLocation());

	if (DistSq < FMath::Square(LODNearRange))
		SetActorTickInterval(0.f);      // 근거리: 매 프레임
	else if (DistSq < FMath::Square(LODFarRange))
		SetActorTickInterval(0.2f);     // 중거리: 5fps
	else
		SetActorTickInterval(0.5f);     // 원거리: 2fps
}
