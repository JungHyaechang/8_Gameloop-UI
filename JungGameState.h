// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "JungGameState.generated.h"

UCLASS()
class ACTORPAWN_API AJungGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	AJungGameState();

	virtual void BeginPlay() override;
	void StartLevel();
	void OnLevelTimeUp();
	void EndLevel();
	void OnCoinCollected();
	void UpdateHUD();
	void StartWave();
	void OnWaveTimeUp();
	void EndWave();
	void ToNextLevel();
	void Wave2();
	void Wave3();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;				   
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	float WaveDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;
	TArray<AActor*> CurrentWaveItem;

	int32 MaxWaves;
	int32 CurrentWaveIndex;

	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;

	UFUNCTION(BlueprintPure, Category = "Score")
	int GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();
};
