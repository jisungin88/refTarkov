// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h" // 가상 탄도 라인을 그리기 위한 헤더
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

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

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false; // 컨트롤러 회전 비활성화
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
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LookAtMouseCursor();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComp->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Shoot);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	//APlayerController* PC = Cast<APlayerController>(GetController());
	const FVector ForwardDirection(1, 0, 0);
	const FVector RightDirection(0, 1, 0);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

//void APlayerCharacter::PlayShootEffects(FVector TraceStart, FVector TraceEnd, bool bHit, FVector HitLocation)
//{
//
//}

void APlayerCharacter::Shoot()
{
	if (!Controller)
		return;

	FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 50;
	FVector EndLocation = StartLocation + (GetActorLocation() * WeaponRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHasHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		Params);

	FVector FinalHitLocation = EndLocation;

	if (bHasHit)
	{
		FinalHitLocation = HitResult.ImpactPoint;
		AActor* HitActor = HitResult.GetActor();

		if (HitActor && HitActor != this)
		{
			UGameplayStatics::ApplyDamage(
				HitActor,
				WeaponDamage,
				GetController(),
				this,
				UDamageType::StaticClass());
		}
	}

	DrawDebugLine(GetWorld(), StartLocation, FinalHitLocation, bHasHit ? FColor::Red : FColor::Green,
		false, 0.1f, 0, 2);

	//PlayShootEffects(StartLocation, FinalHitLocation, bHasHit, FinalHitLocation);
}

void APlayerCharacter::LookAtMouseCursor()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult TraceHitResult;
		// 마우스 커서 아래에 있는 ECC_Visibility의 충돌 정보를 가져옴
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, TraceHitResult))
		{
			FVector TargetLocation = TraceHitResult.ImpactPoint;
			FVector CharacterLocation = GetActorLocation();

			// 바라보는 방향이 땅, 하늘이 안되도록 방지
			TargetLocation.Z = CharacterLocation.Z;

			// 두 좌표 사이의 회전값
			FRotator LookAtRotation = 
				UKismetMathLibrary::FindLookAtRotation(CharacterLocation, TargetLocation);

			// 캐릭터 회전
			SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));
		}

		FVector MouseWorldLocation, MouseWorldDirection;

		if (PC->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
		{
			// 캐릭터가 딛고 있는 가상의 수평 평면 정의 (캐릭터 위치의 Z축 기준, 위를 바라보는 평면)
			FPlane GroundPlane(GetActorLocation(), FVector::UpVector);

			// 가상 평면과 마우스 레이(Ray)의 교차점 계산
			// (MouseWorldLocation에서 MouseWorldDirection 방향으로 뻗어나가 평면과 만나는 지점)
			FVector IntersectionPoint = FMath::LinePlaneIntersection(
				MouseWorldLocation,
				MouseWorldDirection + (MouseWorldDirection * 10000),
				GroundPlane);

			FVector CharacterLocation = GetActorLocation();

			// 회전 계산 시 Z축 높이 차이로 인한 왜곡 방지
			IntersectionPoint.Z = CharacterLocation.Z;

			// 평면 위의 교차점을 바라보도록 회전값 계산 및 적용
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, IntersectionPoint);
			SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
		}
	}
}

