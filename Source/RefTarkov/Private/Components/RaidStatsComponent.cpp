// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RaidStatsComponent.h"
#include "Inventory/DataAssets/ItemDataAsset.h"

void URaidStatsComponent::StartRaid()
{
    // [역할] Raid가 시작된 시점의 월드 시간을 저장한다.
    //        EndRaid에서 "현재 시간 - RaidStartTime" 을 빼면 생존 시간(초)이 나온다.

    RaidStartTime = GetWorld()->GetTimeSeconds();
}

void URaidStatsComponent::EndRaid(ERaidEndReason Reason)
{
    // [역할] Raid 종료 시점에 모든 통계를 CachedSnapshot 하나로 묶는다.
    //        이후 W_RaidResult UMG가 GetSnapshot()으로 이 구조체만 읽으면 된다.
    //        Reason(추출/사망/시간초과)도 함께 기록해 결과 화면 분기에 쓴다.

    CachedSnapshot.SurvivalTimeSeconds = GetWorld()->GetTimeSeconds() - RaidStartTime;
    CachedSnapshot.KillCount = KillCount;
    CachedSnapshot.LootedItems = LootedItems;
    CachedSnapshot.EndReason = Reason;
    CachedSnapshot.TotalXP = CalcXP(CachedSnapshot);
}

void URaidStatsComponent::RecordKill()
{
    // [역할] 적을 처치할 때마다 킬 카운터를 1 올린다.
    //        EndRaid 전까지 누적하다가 Snapshot에 한꺼번에 반영한다.

    KillCount++;
}

void URaidStatsComponent::RecordLoot(const FItemInstance& Item)
{
    // [역할] 인벤토리에 아이템이 성공적으로 추가될 때마다 목록에 기록한다.
    //        W_RaidResult의 "획득 아이템 목록" 표시와 XP 계산 둘 다 이 배열을 기반으로 한다.

    LootedItems.Add(Item);
}

int32 URaidStatsComponent::CalcXP(const FRaidStatsSnapshot& Snap)
{
    // [역할] 통계 수치를 XP 점수로 환산한다.
    //        킬과 아이템 희귀도 두 가지 소스를 합산한다.
    //        공식: 킬 1회 = 50XP, Normal=10 / Rare=25 / Epic=50 / Legendary=100
    //        이 값이 결과 화면의 "획득 XP"로 표시된다.

    int32 XP = Snap.KillCount * 50;
    for (const FItemInstance& Item : Snap.LootedItems)
    {
        if (!Item.ItemDef)
            continue;

        switch (Item.ItemDef->Rarity)
        {
        case EItemRarity::Normal:       XP += 10;   break;
        case EItemRarity::Rare:         XP += 25;   break;
        case EItemRarity::Epic:         XP += 50;   break;
        case EItemRarity::Legendary:    XP += 100;  break;
        }
    }
    return XP;
}
