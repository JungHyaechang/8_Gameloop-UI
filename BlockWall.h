// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockWall.generated.h"

UCLASS()
class ACTORPAWN_API ABlockWall : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlockWall();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USceneComponent* Scene;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent* StaticMesh;
};
