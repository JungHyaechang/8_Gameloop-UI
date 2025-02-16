// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnByCharacter.h"
#include "DronePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "JungGameState.h"
#include "EnhancedInputComponent.h"
#include "JungPlayerController.h"


APawnByCharacter::APawnByCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// ĸ�� ������Ʈ ���� �� ����
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(34.0f, 95.0f);
	CapsuleComponent->SetSimulatePhysics(false);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	// ���̷�Ż �޽� ������Ʈ ���� �� ����
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComp->SetupAttachment(CapsuleComponent);
	SkeletalMeshComp->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleComponent->GetUnscaledCapsuleHalfHeight()));

	bUseControllerRotationYaw = false; // ���콺 ȸ���� ���� Pawn ȸ�� ����ȭ
	// �������� �� ī�޶� �߰�, ����
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(CapsuleComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	OverheadTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadHPTextWidget"));
	OverheadTextWidget->SetupAttachment(SkeletalMeshComp);
	OverheadTextWidget->SetWidgetSpace(EWidgetSpace::Screen);
	
	OverheadBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadHPBarWidget"));
	OverheadBarWidget->SetupAttachment(SkeletalMeshComp);
	OverheadBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	WalkSpeed = NormalSpeed;
	NormalSpeed = 300.0f;
	SprintMultiplier = 3.0f;
	SprintSpeed = NormalSpeed * SprintMultiplier;
	Acceleration = 5.0f;
	Deceleration = 500.0f;
	Gravity = FVector(0.0f, 0.0f, -980.0f);
	bIsGrounded = true;
	bIsSprinting = false;
	MouseSensitivity = 1.5f;
	JumpForce = 700.0f;
	CurrentVelocity = FVector::ZeroVector;
	TargetVelocity = FVector::ZeroVector;
	bCanEnterDrone = false;
	bReverseMoveInput = false;
	CurrentDrone = nullptr;
	TargetYaw = 0.0;
	YawInterpSpeed = 15.0f;

	Maxhealth = 100;
	Health = Maxhealth;
}

void APawnByCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
	UpdateOverheadHPbar();
	WalkSpeed = NormalSpeed;
}

void APawnByCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIfOnGround();

	// �߷� ����
	if (!bIsGrounded)
	{
		// �߷� ���� ���� (�ִ� ���� �ӵ�)
		const float MaxFallSpeed = -3000.0f;
		CurrentVelocity.Z = FMath::Clamp(CurrentVelocity.Z - FMath::Abs(Gravity.Z) * DeltaTime, MaxFallSpeed, JumpForce);
	}
	else
	{
		if (CurrentVelocity.Z <= 0)
		{
			CurrentVelocity.Z = 0;
		}
	}

	// ĳ���� �̵� ������Ʈ ���� , ���� / ���� ó��
	if (!TargetVelocity.IsNearlyZero())
	{
		CurrentVelocity.X = FMath::FInterpTo(CurrentVelocity.X, TargetVelocity.X, DeltaTime, Acceleration);
		CurrentVelocity.Y = FMath::FInterpTo(CurrentVelocity.Y, TargetVelocity.Y, DeltaTime, Acceleration);
	}
	else
	{
		CurrentVelocity.X = FMath::FInterpTo(CurrentVelocity.X, 0.0f, DeltaTime, Deceleration);
		CurrentVelocity.Y = FMath::FInterpTo(CurrentVelocity.Y, 0.0f, DeltaTime, Deceleration);
	}

	FVector FinalVelocity = CurrentVelocity;

	if (!FinalVelocity.IsNearlyZero())
	{
		FVector Newlocation = GetActorLocation() + (FinalVelocity * DeltaTime);
		SetActorLocation(Newlocation, true);
	}
	
	// ĳ���� ȸ�� ������Ʈ ����
	if (Controller) 
	{
		FRotator CarmeraRotation = Controller->GetControlRotation();
		if (!MoveInput.IsNearlyZero()) // �̰� ������ �Է��� ���� ���� ĳ���Ͱ� ���ڿ������� ������ �ٶ�.
		{
			float radians = FMath::Atan2(-1 * MoveInput.Y, MoveInput.X);
			float angles = -1 * radians * (180 / PI);
			TargetYaw = CarmeraRotation.Yaw + angles;
			TargetYaw = fmod(TargetYaw, 360.0);
		}
	}
	
	FRotator CurrentRotaion = GetActorRotation();
	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotaion.Yaw, TargetYaw); // ���� ���� ������ �ذ��ϱ� ���� ���� ����(�ִܰŸ�) 
	float NewYaw = CurrentRotaion.Yaw + DeltaYaw * FMath::Clamp(DeltaTime * YawInterpSpeed, 0.0f, 1.0f); // ����
	CurrentRotaion.Yaw = NewYaw;
	SetActorRotation(CurrentRotaion);
}

void APawnByCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AJungPlayerController* PlayerController = Cast<AJungPlayerController>(GetController()))
		{
			// MoveAction <-> Move ���ε�
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&APawnByCharacter::StartMove
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&APawnByCharacter::StopMove
				);
			}

			// LookAction <-> Look ���ε�
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&APawnByCharacter::Look
				);
			}

			// JumpAction <-> JumpStart, JumpStop ���ε�
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&APawnByCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&APawnByCharacter::StopJump
				);
			}

			// SprintAction <-> SprintStart, SprintStop ���ε�
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&APawnByCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&APawnByCharacter::StopSprint
				);
			}

			// EnterDrone <-> EnterDrone ���ε�
			if (PlayerController->EnterDrone)
			{
				EnhancedInput->BindAction(
					PlayerController->EnterDrone,
					ETriggerEvent::Triggered,
					this,
					&APawnByCharacter::EnterDrone
				);
			}
		}
	}
}

void APawnByCharacter::StartMove(const FInputActionValue& value)
{
	if (!Controller) return;
	MoveInput = value.Get<FVector2D>();

	if (bReverseMoveInput)
	{
		MoveInput *= -1.0f;
	}
	FRotator CarmeraRotaion = Controller->GetControlRotation();
	FVector Forward = FRotationMatrix(CarmeraRotaion).GetUnitAxis(EAxis::X) * MoveInput.X;
	FVector Right = FRotationMatrix(CarmeraRotaion).GetUnitAxis(EAxis::Y) * MoveInput.Y;

	TargetVelocity = Forward + Right;
	TargetVelocity = TargetVelocity.GetSafeNormal() * WalkSpeed;

}
/* ó�� ĳ���� ȸ���� ���� ����ߴ� ����(����)
void  APawnByCharacter::StartRotation(const FInputActionValue& value)
{
	if (!Controller) return;
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!MoveInput.IsNearlyZero())
	{
		FVector Forward = GetActorForwardVector() * MoveInput.X;
		FVector Right = GetActorRightVector() * MoveInput.Y;

		FVector MoveDirection = FVector(Forward.X + Right.X, Forward.Y + Right.Y, 0.0f);

		MoveDirection.Normalize();

		FRotator TargetRotation = MoveDirection.Rotation();
		AddActorLocalRotation(TargetRotation);
	}
}
*/
void APawnByCharacter::StopMove(const FInputActionValue& value)
{
	if (!Controller) return;
	MoveInput = value.Get<FVector2D>();

	if (MoveInput.IsNearlyZero())
	{
		// CurrentVelocity.X = 0;
		// CurrentVelocity.Y = 0;
		TargetVelocity.X = 0;
		TargetVelocity.Y = 0;
		return;
	}
}

void APawnByCharacter::StartJump(const FInputActionValue& value)
{
	bool bJumpPressed = value.Get<bool>();

	if (bJumpPressed && bIsGrounded) // ���鿡 ���� ���� ���� ����
	{
		bIsGrounded = false;
		CurrentVelocity.Z = JumpForce;
		// CurrentVelocity = FVector(CurrentVelocity.X, CurrentVelocity.Y, JumpForce);
	}
}

void APawnByCharacter::StopJump(const FInputActionValue& value)
{
	bool bJumpReleased = !value.Get<bool>();

	if (bJumpReleased && CurrentVelocity.Z > 0) // ���߿� ���� ���� ����Ű���� ���� ���� �� StopJump
	{
		CurrentVelocity.Z *= 0.5f;
	}
}

void APawnByCharacter::CheckIfOnGround()
{
	// ���� üũ�� ���� ����ĳ��Ʈ
	FHitResult Hit;
	FVector Start = GetActorLocation() - FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight());
	FVector End = Start - FVector(0.0f, 0.0f, 10.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // �ڱ� �ڽ��� �浹 ����

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); // ��, �� �� ������ ��ü�� ���� �浹 ä��
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // �����̴� �÷����� ���� �浹 ä��

	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		Start,
		End,
		ObjectQueryParams,
		Params
	);

	if (bHit && Hit.GetActor())
	{
		// UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());
		FVector ImpactPoint = Hit.ImpactPoint;
		float DistanceToGround = (Start.Z - ImpactPoint.Z);

		if (DistanceToGround <= 10.0f)
		{
			bIsGrounded = true;
		}
		else
		{
			bIsGrounded = false;
		}
	}
	else
	{
		bIsGrounded = false;
	}
}

void APawnByCharacter::StartSprint(const FInputActionValue& value)
{
	bool bSprintPressed = value.Get<bool>();

	if (bSprintPressed && !bIsSprinting)
	{
		bIsSprinting = true;
		WalkSpeed = SprintSpeed;
	}
}

void APawnByCharacter::StopSprint(const FInputActionValue& value)
{
	bool bSprintReleased = !value.Get<bool>();

	if (bSprintReleased && bIsSprinting) // Ű�� ���� �޸��� ���� ���� ����
	{
		bIsSprinting = false;
		WalkSpeed = NormalSpeed;
	} 
}

void APawnByCharacter::Look(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D LookInput = value.Get<FVector2D>();
	FRotator CurrentRotation = Controller->GetControlRotation();

	float NewYaw = CurrentRotation.Yaw + (LookInput.X * MouseSensitivity);
	float NewPitch = FMath::Clamp(CurrentRotation.Pitch + (LookInput.Y * MouseSensitivity), -80.0f, 80.0f); // �ִ�, �ּ� ���� ����

	Controller->SetControlRotation(FRotator(NewPitch, NewYaw, 0.0f));

	// ���� ��ȯ �� ��ǥ Ȯ�� �α�
	// UE_LOG(LogTemp, Warning, TEXT("Yaw: %f, Pitch: %f"), NewYaw, NewPitch);
}

void APawnByCharacter::EnterDrone(const FInputActionValue& value)
{
	if (bCanEnterDrone && CurrentDrone)
	{
		AJungPlayerController* PlayerController = Cast<AJungPlayerController>(GetController());
		if (PlayerController)
		{
			// ĳ���� �����
			this->SetActorHiddenInGame(true);
			// �浹 ����
			this->SetActorEnableCollision(false);
			// �Է� ��Ȱ��ȭ
			this->DisableInput(PlayerController);

			PlayerController->Possess(CurrentDrone); // ������ ���� �� CurrentDrone == DronePawn, �ƴ� �� nullptr;	
			if (CurrentDrone)
			{
				// Possess Ȯ�� �α�
				if (PlayerController->GetPawn() == CurrentDrone) 
				{
					UE_LOG(LogTemp, Warning, TEXT("Possess Success"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Possess Fail"));
				}

				CurrentDrone->EnableDroneControll();
				PlayerController->SetViewTargetWithBlend(CurrentDrone, 0.5f);
				UE_LOG(LogTemp, Warning, TEXT("Camera switched"));
			}
		}
	}
}

float APawnByCharacter::GetHealth() const
{
	return Health;
}

void  APawnByCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, Maxhealth);
	UpdateOverheadHP();
	UpdateOverheadHPbar();
}

float APawnByCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamge = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, Maxhealth);
	UpdateOverheadHP();
	UpdateOverheadHPbar();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamge;
}

void APawnByCharacter::OnDeath()
{
	AJungGameState* JungGameState = GetWorld() ? GetWorld()->GetGameState<AJungGameState>() : nullptr;
	if (JungGameState)
	{
		JungGameState->OnGameOver();
	}
}

void APawnByCharacter::UpdateOverheadHP()
{
	if (!OverheadTextWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadTextWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, Maxhealth)));
	}
}

void APawnByCharacter::UpdateOverheadHPbar()
{
	if (!OverheadBarWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadBarWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UProgressBar* HPbar = Cast<UProgressBar>(OverheadWidgetInstance->GetWidgetFromName(TEXT("HealthBar"))))
	{
		float HPpercent = 0.0f;
		if (Maxhealth > 0.0f)
		{
			HPpercent = Health / Maxhealth;
		}

		HPbar->SetPercent(HPpercent);

		if (HPpercent < 0.3f)
		{
			FLinearColor LowHPColor = FLinearColor::Red;
			HPbar->SetFillColorAndOpacity(LowHPColor);
		}
		else if (HPpercent < 0.7f)
		{
			FLinearColor MiddleHPColor = FLinearColor::Yellow;
			HPbar->SetFillColorAndOpacity(MiddleHPColor);
		}
		else
		{
			FLinearColor HighHPColor = FLinearColor::Green;
			HPbar->SetFillColorAndOpacity(HighHPColor);
		}
	}
}

void APawnByCharacter::ReverseMoveInput(bool Reverse)
{
	bReverseMoveInput = Reverse;
}
