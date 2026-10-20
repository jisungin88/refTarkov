#include "AI/EnvQueryTest_CoverScore.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

UEnvQueryTest_CoverScore::UEnvQueryTest_CoverScore()
{
	Cost = EEnvTestCost::High; // 아이템당 트레이스 N회
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(true);
}

void UEnvQueryTest_CoverScore::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	UWorld* World = QueryOwner->GetWorld();
	if (!QueryOwner || !World || SampleHeights.Num() == 0)
	{
		return;
	}

	// Filter 모드 임계값 (Score only 모드에서도 넘겨야하는 인자)
	FloatValueMin.BindData(QueryOwner, QueryInstance.QueryID);
	const float MinThreshold = FloatValueMin.GetValue();
	FloatValueMax.BindData(QueryOwner, QueryInstance.QueryID);
	const float MaxThreshold = FloatValueMax.GetValue();

	// 1. 위협 위치 / 액터
	TArray<FVector> ThreatLocations;
	if (!QueryInstance.PrepareContext(ThreatContext, ThreatLocations) || ThreatLocations.Num() == 0)
	{
		return;
	}
	TArray<AActor*> ThreatActors;
	QueryInstance.PrepareContext(ThreatContext, ThreatActors);

	// 2. 트레이스 파라미터 - 위협 본인과 질의자 캡슐은 제외
	FCollisionQueryParams Params(SCENE_QUERY_STAT(UEnvQueryTest_CoverScore), false);
	Params.AddIgnoredActors(ThreatActors);

	AActor* QuerierActor = Cast<AActor>(QueryOwner);
	if (const AController* Controller = Cast<AController>(QueryOwner))
	{
		QuerierActor = Controller->GetPawn();
	}
	if (QuerierActor)
	{
		Params.AddIgnoredActor(QuerierActor);
	}

	// 3. 아이템별 엄폐도
	const float HeightCount = static_cast<float>(SampleHeights.Num());
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());

		// 위협이 여럿이면 가장 노출된쪽 기준
		float WorstCover = 1.f;
		for (const FVector& ThreatLocation : ThreatLocations)
		{
			const FVector Eye = ThreatLocation + FVector(0.f, 0.f, ThreatEyeHeight);

			int32 BlockedCount = 0;
			for (const float Height : SampleHeights)
			{
				const FVector Sample = ItemLocation + FVector(0.f, 0.f, Height);
				if (World->LineTraceTestByChannel(Eye, Sample, TraceChannel, Params))
				{
					++BlockedCount;
				}
			}
			WorstCover = FMath::Min(WorstCover, BlockedCount / HeightCount);
		}
		It.SetScore(TestPurpose, FilterType, WorstCover, MinThreshold, MaxThreshold);
	}
}

FText UEnvQueryTest_CoverScore::GetDescriptionTitle() const
{
	return FText::FromString(FString::Printf(TEXT("%s: cover from %s"),
	                                         *Super::GetDescriptionTitle().ToString(),
	                                         *UEnvQueryTypes::DescribeContext(ThreatContext).ToString()));
}

FText UEnvQueryTest_CoverScore::GetDescriptionDetails() const
{
	FString Heights;
	for (const float Height : SampleHeights)
	{
		Heights += FString::Printf(TEXT("%.0f "), Height);
	}
	return FText::FromString(FString::Printf(TEXT("Heights: %s| Eye: %.0f\n%s"),
	                                         *Heights, ThreatEyeHeight, *DescribeFloatTestParams().ToString()));
}
