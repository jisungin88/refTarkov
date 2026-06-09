// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "MinionsPlayerState.h"
#include "Components/RaidStatsComponent.h"
#include "Loot/LootContainerActor.h"

AEnemyCharacter::AEnemyCharacter()
{
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TeamId = 1; // 적 팀
}

void AEnemyCharacter::HandleDeath(UHealthComponent* HealthComponent, AController* Killer)
{
	// [역할] 기반 클래스의 래그돌/파괴 처리를 먼저 실행한다.
	//        그 다음, 킬러가 플레이어 컨트롤러일 때만 킬 카운트를 기록한다.
	//        AI끼리 피해를 주고받을 때는 카운트되면 안 된다.
	Super::HandleDeath(HealthComponent, Killer);

	if (APlayerController* PC = Cast<APlayerController>(Killer))
	{
		if (AMinionsPlayerState* PS = PC->GetPlayerState<AMinionsPlayerState>())
		{
			PS->RaidStats->RecordKill();

			if (CorpseClass)
			{
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				GetWorld()->SpawnActor<ALootContainerActor>(CorpseClass, GetActorLocation(), FRotator::ZeroRotator, Params);
			}
		}
	}

}
