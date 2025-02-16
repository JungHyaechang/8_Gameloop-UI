// Fill out your copyright notice in the Description page of Project Settings.


#include "JungGameInstance.h"

UJungGameInstance::UJungGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
}

void UJungGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}
