# RefTarkov — Progress Log

기록 규칙
- 매 세션 끝에 사용자가 직접 한두 줄 추가
- 형식: `## YYYY-MM-DD (Day N / Chapter N)`
- 본문: 한 일 / 막힌 점 / 다음 작업

---

## 2026-05-21 (Day 0 / Setup)
- 한 일: 프로젝트 멘토 프롬프트 작성, Projects 셋업
- 막힌 점: 없음
- 다음 작업: Day 1 — 중복 GameMode 제거, 주석 인코딩 UTF-8 변환, Shoot 버그 수정

---

## 2026-05-22 (Day 1 / Chapter 1 구현 메인)
- 한 일:
  - 정리: Shoot 버그(GetActorLocation → GetActorForwardVector) 수정, LookAtMouseCursor 죽은 코드 제거,
    ATarkovGameModeBase 삭제, MinionsPlayerController 클릭이벤트 끔
  - 무기 시스템(Hybrid):
    - WeaponTypes.h (EAmmoType, EFireMode enum)
    - UWeaponDataAsset (UPrimaryDataAsset, Asset Manager에 "Weapon" 카테고리 등록)
    - AWeaponBase (RPM 게이팅, 탄창, Timer 기반 리로드, virtual Fire(FVector))
    - 마우스 ground 위치 기준 사격 + 가슴 높이 보정으로 탑다운 발/무릎 히트 회피
    - DA_AK74 + BP_Weapon_AK74 인스턴스
  - 헬스: UHealthComponent (OnTakeAnyDamage 표준 경로, OnDeath delegate, GAS-friendly)
  - 캐릭터 리팩토링: AMinionCharacterBase(Abstract) 베이스 + Template Method 사망 처리,
    PlayerCharacter / EnemyCharacter 분리
  - AI: EnemyAIController + AIPerception(Sight) + BB_Enemy + BT_Enemy_Bruiser
    + BTTask_MeleeAttack + EnemyCharacter::PerformMeleeAttack (SweepSingleByChannel)
  - NavMesh 배치
- 막힌 점:
  - 시야각 디버그 시각화 → C++ DrawDebugCone/Circle 직접 구현으로 우회
  - 적 두 마리가 서로 공격 (Team ID 미설정 부작용) — Day 2에서 해결 예정
- 다음 작업: Day 2 — Team ID(IGenericTeamAgentInterface + AttitudeSolver), Chapter 1 종료 작업

---

## 2026-05-23 (Day 2 / Chapter 1 종료)
- 한 일: Team ID 셋업(IGenericTeamAgentInterface + AttitudeSolver), Chapter 1 약점 점검, 면접 1분 스피치 작성
- 막힌 점: 적이 서로 공격하는 이슈 → Team ID로 해결
- 다음 작업: Chapter 2 Day 1 — UItemDataAsset 베이스 + UInventoryComponent 설계
- Chapter 1 회고: 전투 토대 완성. GAS 잘라낸 결정이 일정 보호에 결정적이었음. 청각 인지는 Chapter 2 초반 1시간 작업으로 보강.

---

## 2026-05-24 (Day 3 / Chapter 2 Day 1~2)
- 한 일: 인벤토리 시스템 토대.
  - `UItemDataAsset` 다형성 계층 (Abstract base + `UConsumableItem` → `UHealingItem`, `UAmmoItem` 서브). Identity·Display·WeightKg·MaxStackSize·Rarity 공통 필드, `static const FPrimaryAssetType ItemType` per 서브.
  - `FItemInstance` struct (`TObjectPtr<UItemDataAsset> ItemDef` + `StackCount` + `LoadedAmmo` -1 sentinel)
  - `UInventoryComponent` 스켈레톤 (`TArray<FItemInstance> Slots` + 2-pass `TryAddItem` 알고리즘, slot+weight dual cap)
  - `MinionsCheatManager` (Exec 디버그) — `UDebugComponent`(UActorComponent에 Exec 라우팅 안 됨) 실패 후 `UCheatManager` 표준으로 정착. `DebugAddItem`/`DebugRemoveItemAt`/`DebugDumpInventory`. `DebugItemTable` TMap BP 편집
  - `DefaultGame.ini` PrimaryAssetTypesToScan 등록 (Weapon/Consumable/Ammo)
- 막힌 점: `UActorComponent`에 Exec 안 먹음 — 라우팅 체계(PC→Pawn→CheatManager→PlayerInput)상 `UCheatManager`가 정답.
- 다음 작업: 컨테이너 공유(인벤·stash·loot)

---

## 2026-05-26 (Day 5 / Chapter 2 Day 3)
- 한 일: 컨테이너 3-class 리팩터 (집에서 작업, git merge).
  - `UInventoryContainer` (순수 `UObject`, 모든 로직 + Delegates)
  - `UInventoryComponent` (캐릭터 attach 래퍼)
  - `UStashSubsystem` (`UGameInstanceSubsystem`, 영구 보관, `TransferFromInventory` 제공)
  - 3 곳이 같은 컨테이너 인터페이스 공유 → `ALootContainerActor`도 동일 컨테이너 직접 보유
  - `MaxSlots` 동적 조절 safe-reject (범위에 아이템 있으면 거부)
- 막힌 점: 학원·집 git 동기화 — 이 시점부터 dual env 워크플로 본격화.
- 다음 작업: 루팅 시스템

---

## 2026-05-27 (Day 6 / Chapter 2 Day 4)
- 한 일: 루팅 시스템.
  - `IInteractable` interface (UINTERFACE + IInteractable 2-class 패턴, BlueprintNativeEvent `Interact`/`GetInteractPrompt`/`OnFocusGained`/`OnFocusLost`)
  - `ALootContainerActor` (Mesh + `IInteractable` 구현 + `UInventoryContainer` 직접 보유)
  - `APlayerCharacter` 확장: `InteractionSphere` (USphere, r=150, OverlapAllDynamic) + `OverlappingInteractables` 추적 + `GetBestInteractable` 거리 정렬 + `UpdateInteractableFocus` state machine (Tick에서 focus 전환) + `OnInteract` E 키 binding
  - `ULootTableDataAsset` (`UDataAsset` 상속, `TSoftObjectPtr` 참조 — Asset Manager 카탈로그 불필요, `FLootEntry` struct, weighted random `RollAndAddTo` active pattern, MinRolls/MaxRolls per container)
  - 월드 배치: `BP_LootContainer` 3개 + `DA_LootTable_Box` 어사인. E2E 테스트 OK (E 키 → 전 아이템 transfer, partial remainder 처리)
- 막힌 점: `W_InteractPrompt` UMG 위젯 작업 시도 후 사용자 요청으로 롤백 — `DrawDebugBox` 시각화로 대체. UMG는 Week 3로 미룸.
- 다음 작업: 추출 시스템 (MVP 4·5번)

---

## 2026-06-04 (Day 14 / Chapter 2 종료)

- 한 일: ExtractionZone 신설 + GameMode Observer Pattern 리팩터 (Day 7~14 약 1주 작업).
  - 첫 구현 (감지 + Stash 이동을 ExtractionZone이 직접): 빌드 에러 13개(include 누락, `NotifyFaid`/`Zon`/`Onyl` 등 타입명 오타). 검토 받고 다 잡음.
  - 상용 방향 결정 → Observer Pattern 리팩터:
    - `Types/RaidTypes.h` 신설 (`ERaidPhase`, `ERaidEndReason`)
    - `AMinionsGameModeBase`에 `ERaidPhase` 상태머신 + `EndRaid()` 단일 진입점 + re-entry guard
    - `OnRaidEnded` Dynamic Multicast 도입. Stash 이동은 GameMode 자체 구독자(`HandleRaidEnded_TransferStash`)로 분리
    - `AExtractionZoneActor`는 detector 역할만 — Inventory/Stash 모름
  - `PlayerCharacter::HandleDeath`에 `NotifyRaidFailed` 한 줄 추가 — 인벤 손실은 UObject ownership chain GC로 자동
  - `MinionsCheatManager`에 `DebugDumpStash` 추가
  - 시나리오 A(추출) / B(사망) / C(race condition — guard 동작) 다 검증 OK
- 막힌 점: 코드 작성 단위가 너무 컸음 → 빌드 에러 13개 폭발. Chapter 3 = 파일당 1빌드 원칙.
- 다음 작업: Chapter 3 Day 15 — Cleanup 30분(`ATarkovGameModeBase` 삭제, `FInitialLootEntry` dead code 제거) + `W_RaidResult` UMG 설계

- Chapter 2 회고:
  - **MVP 1~5 완료**: raid 진입 / AI 2종(근접·원거리) / loot→인벤 / 추출→stash / 사망→인벤 손실·stash 보존
  - 주요 산출물: `UItemDataAsset` 다형성 / `UInventoryContainer` 3-class 래퍼(Component·Subsystem·LootActor 공유) / `IInteractable` interface / `ULootTableDataAsset` weighted random / `UCheatManager` Exec 디버그 / Observer Pattern raid-end
  - 일관성: kg float 무게, 슬롯 기반(회전 X), GAS 잘라낸 스코프, Hybrid Asset Manager
  - 면접 카드 누적: Team ID + AttitudeSolver / 3-class container / Observer + 단일 transition + re-entry guard / Detector vs Authority / UObject ownership chain GC
  - 최대 약점: **UMG 경험 0%** — Chapter 3 Day 15~18 정면돌파
  - 워크플로 약점: 다파일 작성 → 빌드 에러 폭발. Chapter 3 = 파일당 1빌드
  - 도구 전환: Claude Code 도입 (Day 14 말). manual 업로드 불필요, dual env git 동기화로 자동

---

## 2026-06-04 (Day 15 / Chapter 3 Day 1)

- 한 일: RaidStats 통계 수집 시스템 전체 구축 + VS 환경 정비.
  - `FRaidStatsSnapshot` 구조체 (`RaidTypes.h` 확장) — KillCount / SurvivalTimeSeconds / LootedItems / TotalXP / EndReason
  - `URaidStatsComponent` 신설 — StartRaid / EndRaid / RecordKill / RecordLoot / CalcXP (Rarity별 차등 XP)
  - `AMinionsPlayerState` 신설 — Pawn 사망 후에도 통계 보존 (PlayerState 수명 = Raid 전체)
  - `AMinionsGameModeBase` 연동 — PlayerStateClass 등록, BeginPlay에서 StartRaid, EndRaid에서 EndRaid 호출
  - `AEnemyCharacter::HandleDeath` override — 킬러가 PlayerController일 때만 RecordKill (AI끼리 제외)
  - `UInventoryComponent::OnItemAdded` Dynamic Multicast Delegate 신설 — Inventory가 통계 시스템 모름
  - `APlayerCharacter::OnInventoryItemAdded` — InventoryComponent와 PlayerState 사이 연결 담당
  - 코드 검토: IWYU 위반 2건 수정, CalcXP Rarity 값 오류 수정, null 체크 보강, 오타 수정
  - VS 환경 정비: .vs / Binaries / Intermediate / Saved 삭제 + 프로젝트 파일 재생성, `.editorconfig` UE5 표준 추가
- 막힌 점: `AConroller` 오타로 빌드 에러 — 파일당 1빌드 원칙 준수했지만 오타는 여전히 조심해야 함
- 다음 작업: Day 16 — `W_RaidResult` UMG 설계 (B형: 추출 성공/실패 + 아이템 목록 + 킬/생존시간/XP 통계)

---

## 2026-06-05 (Day 16 / Chapter 3 Day 2)

- 한 일: UMG 첫 도입 + 메뉴 루프 완성 + Shooter AI C++ 작성
  - `W_RaidResult` / `W_HUD` / `W_MainMenu` / `AMenuPlayerController` C++ 클래스 + BP 에셋 완성
  - HUD Push 바인딩: `OnHealthChanged` (HealthComponent) / `OnAmmoChanged` (WeaponBase) / `OnWeaponEquipped` 지연 바인딩 (OnPossess 타이밍 문제 해결)
  - `HealthComponent::BeginPlay`에서 `OnHealthChanged` broadcast 추가 (초기 HUD 동기화 버그 수정)
  - `UStashSubsystem` LastRaidSnapshot 캐시 — 레벨 전환 후 메뉴에서 지난 레이드 결과 표시
  - `GameMode` HandleRaidEnded_CacheStats 구독자 추가
  - 메뉴 → 레이드 → 결과 화면 → 메뉴 복귀 E2E 루프 검증 완료
  - `AShooterEnemyCharacter` / `UBTTask_ShootAtPlayer` C++ 작성 완료 (빌드 미진행, 내일 이어서)
  - CLAUDE.md: UE5 코딩 컨벤션 / Git 커밋 방식 섹션 추가
- 막힌 점:
  - `APlayerController::Player` 멤버와 파라미터 이름 충돌 (UHT 섀도잉 금지) → `PlayerChar`로 변경
  - `OpenLevel` 레벨 이름만으론 탐색 실패 → 전체 경로(`/Game/...`) 필요
- 다음 작업: Day 17 — `AShooterEnemyCharacter` / `UBTTask_ShootAtPlayer` 빌드 → 에디터 작업 (DA_Enemy_Pistol / BPC_ShooterTarget / EQS_ShooterPosition / BB_Enemy EQSResult 키 추가 / BT_Enemy_Shooter / BP_Enemy_Shooter) → 월드 배치 테스트