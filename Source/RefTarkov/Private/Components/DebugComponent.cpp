// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/DebugComponent.h"
#include "DataAssets/ItemDataAsset.h"

UDebugComponent::UDebugComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UDebugComponent::BeginPlay()
{
	Super::BeginPlay();
	Slots.SetNum(MaxSlots);
}

void UDebugComponent::DebugAddItem(FName Name, int32 Quantity)
{
    // TODO: DebugItemTable에서 Name으로 SoftObjectPtr 찾기
    //   힌트: TSoftObjectPtr* Found = DebugItemTable.Find(Name);

    // TODO: SoftObjectPtr.LoadSynchronous()로 실제 에셋 가져오기

    // TODO: Inventory->TryAddItem(...) 호출, OutRemainder 받음

    // TODO: UE_LOG로 결과 출력 — "Added X, Remainder Y"
}

void UDebugComponent::DebugDumpInventory()
{
    // TODO: Inventory->GetSlots() 순회하면서 각 슬롯 상태 UE_LOG로 출력
    //   힌트: "[%d] %s × %d (Weight: %.1fkg)" 형식
    //   힌트: GetTotalWeight, GetFreeSlotCount도 한 줄에 같이 찍기
}

void UDebugComponent::DebugRemoveItemAt(int32 SlotIndex, int32 Quantity)
{
    // 간단. RemoveItemAt 호출 + UE_LOG
}