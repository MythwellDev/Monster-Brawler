// Fill out your copyright notice in the Description page of Project Settings.


#include "ColossusGameMode.h"
#include "ColossusGameState.h"

AColossusGameMode::AColossusGameMode()
{
	GameStateClass = AColossusGameState::StaticClass();
}