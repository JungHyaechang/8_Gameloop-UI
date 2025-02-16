// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ReversePotion.generated.h"


UCLASS()
class ACTORPAWN_API AReversePotion : public ABaseItem
{
	GENERATED_BODY()

public:
	AReversePotion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DamageAmount;

	virtual void ActivateItem(AActor* Activator) override;
};
