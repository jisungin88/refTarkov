# RefTarkov / Lootlings — Design Notes

## 컨셉 한 줄

반짝이는 건 일단 주워 담는 작은 약탈자 Lootling들이 레이드에서 서로 쏘고 털어 간다.

톤 갭(귀여움 × 잔혹함)이 핵심 셀링 포인트. "루팅 집착"이 세계관이자 게임 루프(루팅 → 무게·슬롯 고민 → 탈출)와 그대로 맞물린다.



## Duckov에서 가져올 것 (IN)

- 탑다운 시점, 마우스 에임

- 단판(raid) 구조 + 추출 지점

- 인벤토리: **슬롯 갯수 제한 + 무게 제한** (그리드 회전 없음, 리스트형 슬롯)

- 죽으면 raid 인벤 손실, 스태시는 영구 보관

- 적 AI **2종 (근접 / 원거리)** — 행동 트리 분기

- 무기별 탄종 구분

- 컨테이너 루팅 (시체, 박스, 서랍)



## Duckov에서 안 가져올 것 (OUT — 영구 제외)

- **인벤토리 회전(테트리스식 그리드 배치)** — 슬롯 + 무게로 충분히 의미 전달

- 멀티플레이어 — 단, 코드는 Replication 친화적으로

- **GAS(Gameplay Ability System) 풀 도입** — 재평가 후 잘라냄. 사유는 아래 "변경 이력" 참고

## 추후 Phase에서 검토할 것

- 무기 모듈러 커스터마이즈(스코프/그립/총열 부착) → **Phase 3 이후 검토**

- 부위별 피격 디테일(팔/다리 부상, 출혈) → **Phase 5 폴리싱 검토**

- 거래/상인/퀘스트 NPC → **미정**

- 의약품/푸드 시스템 → 현재 메디킷 1종만. **Phase 5 검토**



## MVP — 완료 (Day 25 기준)

- [x] 플레이어가 raid에 진입
- [x] 적 AI 2종(근접/원거리)이 플레이어를 인지·추격·공격
- [x] 적/컨테이너에서 루팅 → 슬롯 인벤토리에 적재(슬롯 한도·무게 한도 둘 다 체크)
- [x] 추출 지점에서 N초 대기 → 성공 시 인벤이 스태시로 이동
- [x] 사망 시 raid 인벤 전부 손실, 스태시는 유지
- [x] 메뉴 → raid → 결과 화면 → 메뉴 루프 완성

## Phase별 개발 목표

### Phase 1 — 적 AI 완성 (진행 중)
- `EnvQueryTest_CoverScore` 에디터 연결 — 다중 높이 엄폐도 + 체력 연동 `CoverWeight` (아래 "엄폐 평가" 참고)
- Combat Director / Strafe / Alert / Leash 통합 플레이 테스트 + 수치 밸런싱
- 미완성 정리
  - `WeaponBase::FinishReload` 임시 코드 — 인벤토리 탄약을 소비하지 않고 탄창을 채움 → 탄약 소비 + 부분 재장전 여부 결정 필요
  - 플레이어 사망 처리 (`PlayerCharacter.cpp` 사망 TODO) — 입력 차단 / Ragdoll / 결과 화면 전환
  - Stash 영구 저장 (`USaveGame`) — 저장 시점 결정 필요 (레이드 종료 시 vs 메뉴 진입 시)
  - `StashSubsystem.cpp`에 남은 TODO 주석 정리 (구현은 이미 됨)
- 체크포인트 후 종료

### Phase 2 — 인벤토리 UI
- 인벤토리 창 UMG (슬롯 리스트)
- 루팅 창 UMG (컨테이너 인벤 ↔ 플레이어 인벤 클릭 이동)
- 현재 장비된 무기·아이템 HUD 표시

### Phase 3 — 무기·장비 확장
- `UEquipmentComponent` (무기 슬롯, 아머 슬롯)
- 무기 픽업·교체 (바닥에서 집어서 장착)
- 아머 아이템 (데미지 감소 계수)
- 무기 종 추가 (샷건 산탄, 피스톨 분기)

### Phase 4 — 월드·레이드 맵
- 실제 플레이 가능한 레이드 맵 (실내/실외, 엄폐물 배치)
- 루팅 스폰 랜덤화 (세션마다 다른 아이템 배치)
- 복수 탈출 존

### Phase 5 — 폴리싱
- 사운드 (총성, 피격, UI 효과음)
- VFX Niagara (총구 화염, 피격 이펙트)
- 애니메이션 Blendspace (이동 방향, 조준 Aim Offset)



## 결정 사항 (확정)



### 인벤토리: 슬롯 + 무게

- **슬롯**: 가방마다 고정 슬롯 수 (기본 12칸, 백팩 장착 시 +N칸). 한 슬롯 = 한 종류 스택.

- **무게(kg)**: 아이템마다 단위 무게. 총합이 한계 초과 시 **픽업 거부** (이동 속도 감소 X).

- **회전 없음**: 슬롯은 단순 1칸이므로 회전 개념 자체가 없음. UI도 그리드가 아닌 리스트/격자 슬롯뷰.

- 스태시: 슬롯 수만 다른 동일 구조(무게 제한은 두지 않거나 매우 큼).

- `UInventoryContainer::TryAddItem` 2-pass 알고리즘 — 스택 merge 시도 후 신규 슬롯 추가. 슬롯 한도 & 무게 한도 동시 체크.

- `MaxSlots` 동적 조절 가능하나 범위 내 아이템 있으면 safe-reject.



### 적 AI 2종

| 종류 | 행동 요지 | 무기 | 핵심 BT 분기 |
|---|---|---|---|
| 근접 (Bruiser) | 인지 → 직진 추격 → 인접 시 근접 공격 | 망치/주먹 | Move-to-Target + Attack-In-Range |
| 원거리 (Shooter) | 인지 → 사거리 유지 → EQS로 엄폐 → 사격·재장전 | 피스톨/SMG | Find-Cover + Line-of-Sight + Shoot |

- BT는 공통 부모 BT + 각 종류 서브 트리로 분기(또는 BB 키로 분기)

- AIPerception(Sight + Hearing) 공용

- 근접 적 공격 판정: **SweepSingleByChannel** (캡슐 스윕) — `EnemyCharacter::PerformMeleeAttack`



### 엄폐 평가 — `UEnvQueryTest_CoverScore` (2026-10-20 확정)

- 위협(`ThreatContext`, 보통 `EQSContext_TargetActor`)의 눈높이에서 후보 지점의 여러 높이(`SampleHeights`, 기본 90/45cm)로 `LineTraceTestByChannel`
- 점수 = 막힌 트레이스 비율 → 0(노출) / 0.5(낮은 엄폐물, 앉으면 가려짐) / 1(완전 엄폐)
- 위협이 여럿이면 **최솟값** — 한쪽에서만 가려지는 자리는 엄폐로 치지 않음
- 트레이스에서 위협 액터와 질의자 Pawn은 제외 (안 하면 자기 캡슐에 맞아 전부 "막힘")
- `Cost = High` → EQS가 싼 테스트(Distance, Pathfinding 필터)를 먼저 돌려 후보를 줄인 뒤 실행
- `FloatValueMin/Max` 바인딩 → Filter 모드("엄폐도 0.5 이상만")로도 사용 가능
- **체력 가중치는 테스트 밖에서**: `AEnemyAIController`가 `UHealthComponent::OnHealthChanged`를 구독해 BB `CoverWeight = Lerp(1, MaxCoverWeight, 1 - HP%)` 갱신 → Run EQS Query의 Query Config로 `ScoringFactor` 파라미터에 바인딩. 체력이 낮을수록 같은 쿼리에서 엄폐 비중이 커진다



### 무기 시스템 (Week 1 Day 1~2 완료)

- **Hybrid 설계**: `AWeaponBase` (Actor) + `UWeaponDataAsset` (PrimaryDataAsset) + 차후 `UEquipmentComponent` (Week 2 인벤토리 작업 시 도입)

- 발사: 마우스 ground 위치 기준 trace. 시각 시작점은 머즐, 판정은 가슴 높이로 보정해 탑다운 발/무릎 히트 문제 회피

- 데이터 필드: `RoundsPerMinute`, `BaseDamage`, `Range`, `AmmoType`, `MagazineSize`, `ReloadTime`, `PenetrationPower`(Week 3 자리), `WeightKg`

- Enhanced Input: AttackAction = `Triggered`(자동 발사 대응), ReloadAction = `Started`

- 다형성: `virtual Fire(FVector TargetLocation)` — 샷건 산탄 등 서브클래스 확장 가능. AI도 같은 인터페이스 사용



### 헬스/데미지/사망

- `UHealthComponent` 분리. Owner 액터의 `OnTakeAnyDamage` 멀티캐스트 델리게이트 구독으로 표준 경로 수신

- 핵심 인터페이스: `CurrentHealth`, `MaxHealth`, `ApplyHealing(float)`, `OnHealthChanged`, `OnDeath`, `IsAlive()`

- 데미지 입력은 항상 외부에서 `UGameplayStatics::ApplyDamage` 또는 `AActor::TakeDamage`. HealthComponent의 직접 데미지 API는 노출하지 않음(GAS 마이그레이션 시 호환 위함)

- 사망 처리:

  - 공통: 입력/사격 차단 → Ragdoll on → 사망 효과(사운드/파티클 placeholder) → 5초 후 Destroy

  - AI 적: 위 + 루트 가능한 컨테이너 액터 스폰

  - 플레이어: 위 + `NotifyRaidFailed` 한 줄 → GameMode가 EndRaid 처리

- 데미지 타입: 기본 `UDamageType`만. 헤드샷/관통은 Week 3에 서브클래스 분기



### 인벤토리 컨테이너 3-class 아키텍처 (Day 5 확정)

- `UInventoryContainer` (순수 `UObject` — 모든 로직 + Delegates)

- `UInventoryComponent` (캐릭터 attach 래퍼)

- `UStashSubsystem` (`UGameInstanceSubsystem`, raid 간 영구 보관, `TransferFromInventory` 제공)

- `ALootContainerActor`도 동일 `UInventoryContainer` 직접 보유 → 셋 다 같은 인터페이스 공유



### 인터랙션 시스템 (Day 6 확정)

- UINTERFACE + IInteractable 2-class 패턴 (UE 표준)

- BlueprintNativeEvent: `Interact` / `GetInteractPrompt` / `OnFocusGained` / `OnFocusLost`

- `APlayerCharacter::InteractionSphere` (`USphereComponent`, r=150) + `OverlappingInteractables` 추적

- `GetBestInteractable`: 거리 기준 정렬 → 가장 가까운 것 선택 (Tick에서 focus state machine)



### 루팅 시스템 (Day 6 확정)

- `ULootTableDataAsset` (`UDataAsset` 상속)

- `TSoftObjectPtr` 참조 — Asset Manager 카탈로그 등록 불필요

- `FLootEntry`: ItemDef + Weight (정규화 필요 없는 weighted random)

- `RollAndAddTo` active pattern, `MinRolls`/`MaxRolls` per container



### Raid 상태머신 + Observer Pattern (Day 14 확정)

- `Types/RaidTypes.h`: `ERaidPhase`, `ERaidEndReason`

- `ALootlingsGameModeBase`: `ERaidPhase` 상태머신 + `EndRaid()` 단일 진입점 + re-entry guard

- `OnRaidEnded` Dynamic Multicast Delegate

- `AExtractionZoneActor` = Detector 역할만 (Inventory/Stash 모름) → Authority는 GameMode

- Stash 이동은 GameMode 자체 구독자 `HandleRaidEnded_TransferStash`로 분리

- 인벤 손실: `UObject` ownership chain GC 자동 처리



### 통계 수집 (Day 15 결정)

- `URaidStatsComponent` (재사용성 우선) — 플레이어 캐릭터 또는 PlayerState에 Attach

- 추적 항목: 킬 수 / 생존 시간 / 획득 아이템 목록 / 획득 XP

- `W_RaidResult` UMG가 이 컴포넌트를 읽어 표시



## 변경 이력



### 2026-10-20 — IP 교체: Minions → Lootlings

- **사유**: Minions는 타사 IP라 공개 포트폴리오·영상에 쓸 수 없음. 이름만 쓰고 있었고 전용 에셋은 없어서 교체 비용이 작은 지금 바꿈
- **범위**: C++ 클래스 5개(`*GameModeBase` / `*PlayerController` / `*PlayerState` / `*CheatManager` / `ALootlingCharacterBase`) + BP 3개(`BP_*GameMode` / `BP_*Controller` / `BP_*CheatManager`) + 문서
- 기존 BP 참조는 `DefaultEngine.ini`의 `[CoreRedirects]` `+ClassRedirects`로 유지
- 모듈명·프로젝트명 `RefTarkov`는 그대로 (내부 이름, 바꾸면 비용 대비 이득 없음)



### 2026-10-20 — CoverScore 재설계 (원안 변경)

- **원안**: 체력이 낮을수록 점수에 `HealthMultiplier`(1.0~2.0)를 곱하는 테스트
- **문제**: EQS는 테스트마다 아이템 점수를 min-max 정규화한다(`UEnvQueryTest::NormalizeItemScores`, `Runtime/AIModule/Private/EnvironmentQuery/EnvQueryTest.cpp`). 모든 아이템에 같은 상수를 곱하면 정규화에서 상쇄돼 결과가 바뀌지 않음 — 그리고 정작 "엄폐"를 재는 로직이 없었음
- **변경**: 실제 엄폐도를 재는 다중 높이 트레이스 테스트로 교체 + 체력 가중치는 `ScoringFactor` 파라미터 바인딩으로 이동 (결정 사항 "엄폐 평가" 참고)
- **면접 포인트**: "가중치를 곱했는데 결과가 안 바뀌는 이유를 엔진 소스에서 찾아 설계를 바꿨다" — 테스트 간 상대 비중은 ScoringFactor가 정한다



### 2026-05-22 — GAS 풀 도입 잘라냄 (원안 변경)

- **원안**: Phase 1~3 단계적 GAS 도입 (`UAbilitySystemComponent` + `UAttributeSet` + `UGameplayEffect` + `UGameplayAbility`)

- **변경**: GAS 풀 도입 잘라냄. 3주 일정 안에 익스트랙션 슈터 핵심 루프(루팅·인벤토리·익스트랙션 사이클) 완성을 우선

- **근거**:

  (a) GAS 학습 곡선이 크다 — 동시 학습이 필요한 개념이 6~7개(AbilitySystemComponent, AttributeSet, GameplayEffect, GameplayCue, GameplayTag, Activation/Net Policy). design_notes 원안에도 명시했던 위험

  (b) GAS는 ActionRPG / 격투 / MOBA 장르에서 더 가치 있는 시스템. 익스트랙션 슈터의 핵심 차별점이 아님

  (c) Week 1 절반이 지난 시점에 도입 시 인벤토리·익스트랙션 일정이 깨질 가능성 큼

- **마이그레이션 경로 보존**: 현재 코드는 GAS-friendly 표준 경로를 따른다 — `UGameplayStatics::ApplyDamage` → `AActor::TakeDamage` → `OnTakeAnyDamage` 델리게이트. `UHealthComponent`를 향후 `UAttributeSet`으로 교체 시 외부 인터페이스 거의 그대로

- **면접 포지셔닝**: "초기엔 GAS 도입을 계획했으나, 익스트랙션 슈터의 핵심에 우선 집중하기 위해 가벼운 컴포넌트 기반으로 결정. 단, 마이그레이션 friendly한 표준 경로를 유지해 향후 v2에서 GAS로 옮길 수 있는 구조." → 스코프 관리 능력으로 어필



### 2026-05-27 — W_InteractPrompt UMG 롤백 (Day 6)

- **원안**: E 키 인터랙션 시 UMG 프롬프트 위젯 표시

- **변경**: `DrawDebugBox` 시각화로 대체, UMG는 Week 3(Chapter 3)으로 전면 연기

- **근거**: Chapter 2 핵심 루프(루팅/추출/사망) 완성 우선. UMG 학습 비용을 Week 3에 몰아서 정면돌파



## Chapter 3 — 완료 (Day 15~21)

- [x] `W_RaidResult` (추출 성공/실패, 아이템 목록, 킬/생존시간/XP, 확인 버튼 → 메뉴)
- [x] `W_HUD` (체력바, 탄약 카운터)
- [x] `W_MainMenu` + `AMenuPlayerController`
- [x] 메뉴 → raid → 결과 → 메뉴 E2E 루프 완성



## 잔여 결정 항목

- [x] 무게 초과 시: **픽업 거부** (이동 속도 감소 X)

- [x] 슬롯 기본 갯수: **12칸** (MaxSlots 기본값)

- [x] 무게 단위 → **kg (float)**. `UWeaponDataAsset::WeightKg`로 결정

- [x] 근접 적 공격 판정 방식: **SweepSingleByChannel** (캡슐 스윕)



## Lootlings 톤 가이드

- 캐릭터: 2~3등신 작은 생물 + 몸만 한 가방(루팅 집착을 실루엣으로 표현). 무료 에셋 기반
- **금지**: 노란 피부 + 고글 + 멜빵 조합 등 기존 Minions를 연상시키는 디자인

- 색감: 채도 높은 원색(팀·적 구분은 가방·장비 색으로), 피·이펙트는 만화적으로(붉은 잉크 톤)

- 사운드: 코믹한 비명/총성 (저작권 무료 SFX)

- 메뉴 UI: 두꺼운 카툰 보더, Bangers/Luckiest Guy 류 폰트 (Comic Sans 금지)



## 외부 에셋 출처 (라이선스 확인용)

- (채워나갈 것)
