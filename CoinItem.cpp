// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "Engine/World.h"
#include "JungGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefaultCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (AJungGameState* GameState = World->GetGameState<AJungGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}

		DestroyItem();
	}
}
