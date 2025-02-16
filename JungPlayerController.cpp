// Fill out your copyright notice in the Description page of Project Settings.


#include "JungPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "JungGameState.h"
#include "JungGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


AJungPlayerController::AJungPlayerController()
	: CharacterIMC(nullptr),
	DroneIMC(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr),
	DroneMove(nullptr),
	EnterDrone(nullptr),
	HUDWidgetInstance(nullptr),
	HUDWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr)
{
}

void AJungPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// 캐릭터 IMC 활성화
			if (CharacterIMC)
			{
				Subsystem->AddMappingContext(CharacterIMC, 0);
			}
		}
	}
	
	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

UUserWidget* AJungPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AJungPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromViewport();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromViewport();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("TotalScore: %d"), JungGameInstance->TotalScore)));
				}
			}
		}
	}
}

void AJungPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromViewport();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromViewport();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		AJungGameState* JungGameState = GetWorld() ? GetWorld()->GetGameState<AJungGameState>() : nullptr;
		if (JungGameState)
		{
			JungGameState->UpdateHUD();
		}
	}
}

void AJungPlayerController::StartGame()
{
	if (UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		JungGameInstance->CurrentLevelIndex = 0;
		JungGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}






