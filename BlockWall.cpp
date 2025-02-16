// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockWall.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABlockWall::ABlockWall()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMesh->SetupAttachment(Scene);
	StaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

