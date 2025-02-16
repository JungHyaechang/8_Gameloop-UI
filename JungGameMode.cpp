// Fill out your copyright notice in the Description page of Project Settings.


#include "JungGameMode.h"
#include "PawnByCharacter.h"
#include "JungPlayerController.h"
#include "JungGameState.h"

AJungGameMode::AJungGameMode()
{
	DefaultPawnClass = APawnByCharacter::StaticClass();
	PlayerControllerClass = AJungPlayerController::StaticClass();
	GameStateClass = AJungGameState::StaticClass();
}
