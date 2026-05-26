// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionsGameModeBase.h"
#include "GenericTeamAgentInterface.h"

void AMinionsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	FGenericTeamId::SetAttitudeSolver([](FGenericTeamId A, FGenericTeamId B)
	{
		if (A == B)
			return ETeamAttitude::Friendly;
		else if (A == 254 || B == 254)
			return ETeamAttitude::Neutral;
		else
			return ETeamAttitude::Hostile;
	});
}
