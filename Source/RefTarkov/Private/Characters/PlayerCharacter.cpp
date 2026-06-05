// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/HealthComponent.h"
#include "Weapons/WeaponBase.h"
#include "Loot/Interactable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MinionsGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "MinionsPlayerState.h"
#include "Components/RaidStatsComponent.h"
#include "Inventory/Items/ItemInstance.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 기본 회전 옵션 끄기 (마우스 방향으로 수동 회전시킬 것이기 때문)
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // 캐릭터 회전에 영향을 안받음
	CameraBoom->TargetArmLength = 1000; // 카메라 높이(길이)
	CameraBoom->SetRelativeRotation(FRotator(-60, 0, 0)); // 카메라 각도
	CameraBoom->bDoCollisionTest = false; // 콜리전에 영향을 받지 않음?
	CameraBoom->bEnableCameraLag = true;  // 카메라가 캐릭터를 부드럽게 추적하는 기능 활성화
	CameraBoom->CameraLagSpeed = 5.0f;    // 수치가 낮을수록 더 묵직하고 부드럽게 따라옴 (3.0 ~ 5.0 추천)

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Sphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false; // 컨트롤러 회전 비활성화

	TeamId = 0;

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	if (DefaultWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;
		Params.Instigator = this;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(
			DefaultWeaponClass,
			GetActorTransform(),
			Params);

		if (CurrentWeapon)
		{
			const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
			CurrentWeapon->AttachToComponent(GetMesh(), Rules, TEXT("hand_r"));
		}
	}

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnInteractSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnInteractSphereEndOverlap);

	InventoryComp->OnItemAdded.AddDynamic(this, &APlayerCharacter::OnInventoryItemAdded);
	OnWeaponEquipped.Broadcast(CurrentWeapon);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAimFromMouse();
	UpdateInteractableFocus();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OnFireInput);
		EnhancedInputComp->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::OnReloadInput);
		EnhancedInputComp->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::OnInteract);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	const FVector ForwardDirection(1, 0, 0);
	const FVector RightDirection(0, 1, 0);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::OnFireInput()
{
	if (!CurrentWeapon) return;
	CurrentWeapon->Fire(CachedMouseGroundLocation);
}

void APlayerCharacter::OnReloadInput()
{
	if (!CurrentWeapon) return;
	CurrentWeapon->StartReload();
}

void APlayerCharacter::HandleDeath(class UHealthComponent* HealthComponent, AController* Killer)
{
	Super::HandleDeath(HealthComponent, Killer);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorEnableCollision(false);
	}

	if (AMinionsGameModeBase* GM = Cast<AMinionsGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GM->NotifyRaidFailed(this);
	}

	// TODO 사운드/파티클/실패처리
}

void APlayerCharacter::UpdateAimFromMouse()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	FVector MouseWorldLocation, MouseWorldDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
		return;

	// 가상 Plane
	const FPlane GoundPlane(GetActorLocation(), FVector::UpVector);

	const FVector IntersectionPoint = FMath::LinePlaneIntersection(
		MouseWorldLocation,
		MouseWorldLocation + MouseWorldDirection * 10000,
		GoundPlane);

	CachedMouseGroundLocation = IntersectionPoint;

	const FVector CharacterLocation = GetActorLocation();
	FVector AimXY = IntersectionPoint;
	AimXY.Z = CharacterLocation.Z;

	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, AimXY);
	SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));
}

void APlayerCharacter::OnInteractSphereBeginOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep)
{
	if (!Other) return;
	if (Overlapped != InteractionSphere) return;

	if (Other->Implements<UInteractable>())
	{
		OverlappingInteractables.AddUnique(Other);
	}
}

void APlayerCharacter::OnInteractSphereEndOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Other) return;
	if (Overlapped != InteractionSphere) return;

	if (Other->Implements<UInteractable>())
	{
		OverlappingInteractables.Remove(Other);
	}
}

AActor* APlayerCharacter::GetBestInteractable() const
{
	AActor* NearTarget = nullptr;
	float MinDistance = MAX_flt;
	FVector MyLocation = GetActorLocation();

	for (AActor* InteractTarget : OverlappingInteractables)
	{
		if (!IsValid(InteractTarget))
			continue;

		float TargetDist = FVector::DistSquared(MyLocation, InteractTarget->GetActorLocation());
		if (MinDistance > TargetDist)
		{
			MinDistance = TargetDist;
			NearTarget = InteractTarget;
		}
	}

	return NearTarget;
}

void APlayerCharacter::OnInteract()
{
	if (AActor* Target = GetBestInteractable())
	{
		IInteractable::Execute_Interact(Target, this);
		// ↑ 이 호출이 UE 인터페이스의 "올바른" 방식. 직접 호출 X.
	}
}

void APlayerCharacter::UpdateInteractableFocus()
{
	AActor* NewNearTarget = GetBestInteractable();
	AActor* OldTarget = CurrentFocusedInteractable.Get();

	if (NewNearTarget == OldTarget) return;

	if (OldTarget && OldTarget->Implements<UInteractable>())
	{
		IInteractable::Execute_OnFocusLost(OldTarget, this);
	}

	if (NewNearTarget && NewNearTarget->Implements<UInteractable>())
	{
		IInteractable::Execute_OnFocusGained(NewNearTarget, this);
	}

	CurrentFocusedInteractable = NewNearTarget;
}

void APlayerCharacter::OnInventoryItemAdded(UItemDataAsset* ItemDef, int32 Quantity)
{
	if (AMinionsPlayerState* PS = GetPlayerState<AMinionsPlayerState>())
	{
		FItemInstance Instance;
		Instance.ItemDef = ItemDef;
		Instance.StackCount = Quantity;
		PS->RaidStats->RecordLoot(Instance);
	}
}
