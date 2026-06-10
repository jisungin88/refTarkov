// Fill out your copyright notice in the Description page of Project Settings.


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

const FName AEnemyAIController::TargetActorKey(TEXT("TargetActor"));
const FName AEnemyAIController::SuspicionTargetKey(TEXT("SuspicionTarget"));
const FName AEnemyAIController::InvestigateLocationKey(TEXT("InvestigateLocation"));

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
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 600.f;

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
	}

	// BT 시작
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn))
	{
		if (UBehaviorTree* BT = Enemy->GetBehaviorTree())
		{
			RunBehaviorTree(BT);
		}
	}
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
			Blackboard->ClearValue(TargetActorKey);
		else if (bHearing)
			Blackboard->ClearValue(InvestigateLocationKey);
	}
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	DrawSightDebug();
#endif
}

#if WITH_EDITOR
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
