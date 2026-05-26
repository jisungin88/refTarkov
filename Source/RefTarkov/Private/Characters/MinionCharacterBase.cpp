// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MinionCharacterBase.h"
#include "Components/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMinionCharacterBase::AMinionCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
}

void AMinionCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (HealthComp)
	{
		HealthComp->OnDeath.AddDynamic(this, &AMinionCharacterBase::HandleDeath);
	}
}

bool AMinionCharacterBase::IsAlive() const
{
	return HealthComp ? HealthComp->IsAlive() : false;
}

void AMinionCharacterBase::HandleDeath(UHealthComponent* HealthComponent, AController* Killer)
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
		MoveComp->StopMovementImmediately();
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetSimulatePhysics(true);
	}

	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetLifeSpan(5);
}

