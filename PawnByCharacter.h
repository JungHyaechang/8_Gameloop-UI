// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnByCharacter.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class ADronePawn;
struct FInputActionValue;

UCLASS()
class ACTORPAWN_API APawnByCharacter : public APawn
{
	GENERATED_BODY()

public:
	
	APawnByCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RootComponent")
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadTextWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadBarWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	bool bCanEnterDrone;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ADronePawn* CurrentDrone;

	bool bReverseMoveInput;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);
	void ReverseMoveInput(bool Reverse);
protected:
	
	virtual void BeginPlay() override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
	void OnDeath();
	void UpdateOverheadHP();
	void UpdateOverheadHPbar();

	UFUNCTION()
	void StartMove(const FInputActionValue& value);
	UFUNCTION()
	void StopMove(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void CheckIfOnGround();
	UFUNCTION()
	void EnterDrone(const FInputActionValue& value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector CurrentVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector TargetVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TargetYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float YawInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float JumpForce;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sensitivity")
	float MouseSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Acceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Deceleration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity")
	FVector Gravity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity")
	bool bIsGrounded;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	FVector2D MoveInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Maxhealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;
};
