// Fill out your copyright notice in the Description page of Project Settings.


#include "ReversePotion.h"
#include "PawnByCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AReversePotion::AReversePotion()
{
	DamageAmount = 10;
}

void AReversePotion::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	TArray<AActor*> OverlappingActors;
	CollisionComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				DamageAmount,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (APawnByCharacter* PlayerCharacter = Cast<APawnByCharacter>(Activator))
		{
			PlayerCharacter->ReverseMoveInput(true);
		}

		DestroyItem();
	}
}
