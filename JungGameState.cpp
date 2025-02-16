// Fill out your copyright notice in the Description page of Project Settings.


#include "JungGameState.h"
#include "JungGameInstance.h"
#include "JungPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

AJungGameState::AJungGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 60.0f;
	CurrentLevelIndex = 0;
	CurrentWaveIndex = 0;
	MaxLevels = 3;
	MaxWaves = 3;
}

void AJungGameState::BeginPlay()
{
	Super::BeginPlay();
	
	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&AJungGameState::UpdateHUD,
		0.1f,
		true
	);
}

void AJungGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AJungPlayerController* JungPlayerController = Cast<AJungPlayerController>(PlayerController))
		{
			JungPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(GameInstance);
		if (JungGameInstance)
		{
			CurrentLevelIndex = JungGameInstance->CurrentLevelIndex;
		}
	}

	CurrentWaveIndex = 0;
	StartWave();
}

int AJungGameState::GetScore() const
{
	return Score;
}

void AJungGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(GameInstance);
		if (JungGameInstance)
		{
			JungGameInstance->AddToScore(Amount);
		}
	}
}

void AJungGameState::OnLevelTimeUp()
{
	EndLevel();
}

void AJungGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void AJungGameState::EndLevel()
{
	// 파티클이 2초뒤에 사라지는데 그 사이에 다름 레벨로 넘어가면 오류가 생김
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		FTimerHandle OpenTimerHandle;
		GetWorldTimerManager().SetTimer(
			OpenTimerHandle,
			[this]()
			{
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			},
			3.0f,
			false
		);
	}
	else
	{
		OnGameOver();
	}
}

void AJungGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AJungPlayerController* JungPlayerController = Cast<AJungPlayerController>(PlayerController))
		{
			JungPlayerController->ShowMainMenu(true);
			JungPlayerController->SetPause(true);
		}
	}
}


void AJungGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AJungPlayerController* JungPlayerController = Cast<AJungPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = JungPlayerController->GetHUDWidget())
			{
				// TimeText
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				// ScoreText
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TotalScore"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(GameInstance);
						if (JungGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), JungGameInstance->TotalScore)));
						}
					}
				}

				// LevelText
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}

				// WaveText
				if (UTextBlock* WaveIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWaveIndex + 1)));
				}
			}
		}
	}
}

void AJungGameState::StartWave()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	for (AActor* ItemToDestroy : CurrentWaveItem)
	{
		if (ItemToDestroy && ItemToDestroy->IsValidLowLevelFast()) // 여전히 메모리상에 유효한 상태로 있는것인지 확인 (가비지 컬렉션)
		{
			ItemToDestroy->Destroy();
		}
	}
	CurrentWaveItem.Empty();

	TArray<AActor*> FoundVolumes;
	// 현재 월드에서 해당 액터에 관한 모든 액터들을 가져와서 FoundVolume에 저장
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40;

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor)
				{
					CurrentWaveItem.Add(SpawnedActor);

					if (SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
			}
		}
	}

	switch (CurrentWaveIndex)
	{
	case 1:
		Wave2();
		break;
	case 2:
		Wave2();
		Wave3();
		break;
	default:
		break;
	}

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&AJungGameState::OnWaveTimeUp,
		WaveDuration,
		false
	);
}

void AJungGameState::OnWaveTimeUp()
{
	OnGameOver();
}

void AJungGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(GameInstance);
		if (JungGameInstance)
		{
			AddScore(Score);
		}
	}

	CurrentWaveIndex++;

	if (CurrentWaveIndex >= MaxWaves)
	{
		ToNextLevel();
	}
	else 
	{
		StartWave();
	}
}

void AJungGameState::ToNextLevel()
{
	CurrentLevelIndex++;
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UJungGameInstance* JungGameInstance = Cast<UJungGameInstance>(GameInstance))
		{
			JungGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}
	EndLevel();
}

void AJungGameState::Wave2()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < 7; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnActor = SpawnVolume->SpawnReverseItem();
				if (SpawnActor)
				{
					CurrentWaveItem.Add(SpawnActor);
				}
			}
		}
	}
}

void AJungGameState::Wave3()
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < 10; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnActor = SpawnVolume->SpawnWall();
				if (SpawnActor)
				{
					CurrentWaveItem.Add(SpawnActor);
				}
			}
		}
	}
}
