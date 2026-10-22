// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_CoverScore.generated.h"

UCLASS()
class REFTARKOV_API UEnvQueryTest_CoverScore : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_CoverScore();

protected:
	//~ UEnvQueryTest interface
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
	//~ End UEnvQueryTest interface

	/** 위협 위치 */
	UPROPERTY(EditDefaultsOnly, Category = "Cover")
	TSubclassOf<UEnvQueryContext> ThreatContext;

	/** 위협의 눈높이 */
	UPROPERTY(EditDefaultsOnly, Category = "Cover", meta = (ClampMin = "0.0"))
	float ThreatEyeHeight = 60.f;

	/** 아이템 위치에서 검사할 높이들 */
	UPROPERTY(EditDefaultsOnly, Category = "Cover")
	TArray<float> SampleHeights = { 90.f, 45.f };

	UPROPERTY(EditDefaultsOnly, Category = "Cover")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
};
