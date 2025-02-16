// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingItem.h"
#include "PawnByCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (APawnByCharacter* PlayerCharacter = Cast<APawnByCharacter>(Activator))
		{
			PlayerCharacter->AddHealth(HealAmount);
			PlayerCharacter->ReverseMoveInput(false);
		}

		DestroyItem();
	}
}
