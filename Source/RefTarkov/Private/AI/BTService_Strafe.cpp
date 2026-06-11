// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_Strafe.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTService_Strafe::UBTService_Strafe()
{
	NodeName = TEXT("Strafe");
}

uint16 UBTService_Strafe::GetInstanceMemorySize() const
{
	return sizeof(FServiceMemory);
}

void UBTService_Strafe::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC || AIC->IsFollowingAPath())
		return;

	ACharacter* Pawn = Cast<ACharacter>(AIC->GetPawn());
	if (!Pawn)
		return;

	FServiceMemory* Mem = reinterpret_cast<FServiceMemory*>(NodeMemory);
	Mem->Timer -= DeltaSeconds;
	if (Mem->Timer <= 0.f)
	{
		Mem->Direction *= -1.f;
		Mem->Timer = DirectionChangePeriod;
	}

	Pawn->AddMovementInput(Pawn->GetActorRightVector() * Mem->Direction);
}
