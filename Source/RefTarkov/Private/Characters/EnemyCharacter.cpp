// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Inventory/InventoryContainer.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/Items/ItemInstance.h"
#include "Loot/LootTableDataAsset.h"
#include "MinionsPlayerState.h"
#include "Components/RaidStatsComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	TeamId = 1;

	CorpseDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CorpseDetection"));
	CorpseDetectionSphere->SetupAttachment(RootComponent);
	CorpseDetectionSphere->SetSphereRadius(100.f);
	CorpseDetectionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CorpseDetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyCharacter::HandleDeath(UHealthComponent* HealthComponent, AController* Killer)
{
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement();
		if (AIC->BrainComponent)
		{
			AIC->BrainComponent->StopLogic(TEXT("Dead"));
		}
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
	}

	Super::HandleDeath(HealthComponent, Killer);
	SetLifeSpan(120.f);

	if (APlayerController* PC = Cast<APlayerController>(Killer))
	{
		if (AMinionsPlayerState* PS = PC->GetPlayerState<AMinionsPlayerState>())
		{
			PS->RaidStats->RecordKill();
		}
	}

	InitCorpseLoot();

	CorpseDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CorpseDetectionSphere->UpdateOverlaps();
}

void AEnemyCharacter::InitCorpseLoot()
{
	CorpseLoot = NewObject<UInventoryContainer>(this);
	CorpseLoot->Initialize(CorpseSlotCount, -1.f);

	if (ULootTableDataAsset* Table = LootTable.LoadSynchronous())
	{
		Table->RollAndAddTo(CorpseLoot);
	}
}

void AEnemyCharacter::Interact_Implementation(AActor* Interactor)
{
	if (bAlreadyLooted || !CorpseLoot || !Interactor)
		return;

	UInventoryComponent* PlayerInv = Interactor->FindComponentByClass<UInventoryComponent>();
	if (!PlayerInv)
		return;

	const TArray<FItemInstance>& Items = CorpseLoot->GetSlots();
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		const FItemInstance& Item = Items[i];
		if (Item.IsEmpty())
			continue;

		int32 Remainder = 0;
		PlayerInv->TryAddItem(Item.GetItemDef(), Item.StackCount, Remainder);

		const int32 Moved = Item.StackCount - Remainder;
		if (Moved > 0)
			CorpseLoot->RemoveItemAt(i, Moved);
	}

	bAlreadyLooted = true;
	CorpseDetectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!IsAlive())
		return Damage;

	AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());
	UBlackboardComponent* BB = AIC ? AIC->GetBlackboardComponent() : nullptr;
	if (!BB)
		return Damage;

	if (BB->GetValueAsObject(AEnemyAIController::TargetActorKey))
		return Damage;

	FVector SuspectLocation = FVector::ZeroVector;
	if (DamageCauser)
	{
		SuspectLocation = DamageCauser->GetActorLocation();
	}
	else if (EventInstigator && EventInstigator->GetPawn())
	{
		SuspectLocation = EventInstigator->GetPawn()->GetActorLocation();
	}

	if (!SuspectLocation.IsZero())
	{
		BB->SetValueAsVector(AEnemyAIController::InvestigateLocationKey, SuspectLocation);
	}

	return Damage;
}

FText AEnemyCharacter::GetInteractPrompt_Implementation() const
{
	return bAlreadyLooted
		? FText::FromString(TEXT("Already, Searched"))
		: FText::FromString(TEXT("Search Body"));
}

void AEnemyCharacter::OnFocusGained_Implementation(AActor* Interactor)
{

}

void AEnemyCharacter::OnFocusLost_Implementation(AActor* Interactor)
{

}
