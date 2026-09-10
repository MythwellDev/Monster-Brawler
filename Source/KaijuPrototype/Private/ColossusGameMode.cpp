// Fill out your copyright notice in the Description page of Project Settings.


#include "ColossusGameMode.h"




AColossusGameMode::AColossusGameMode()
{
	GameStateClass = AColossusGameState::StaticClass();
}

void AColossusGameMode::BeginNextRound()
{
	AColossusGameState* ColossusGameState = GetColossusGameState();

	if (!ColossusGameState) return;

	const int32 NextRound = ColossusGameState->GetCurrentRound() + 1;
	
	if (NextRound > ColossusGameState->GetTotalRounds())
	{
		return;
	}

	ColossusGameState->SetRoundState(NextRound, ColossusGameState->GetTotalRounds(), EColossusRoundPhase::Intro);
}

void AColossusGameMode::ActivateCurrentRound()
{
	AColossusGameState* ColossusGameState = GetColossusGameState();
	if (!ColossusGameState)
	{
		return;
	}

	ColossusGameState->SetRoundState(ColossusGameState->GetCurrentRound(), ColossusGameState->GetTotalRounds(), EColossusRoundPhase::Active);
}

bool AColossusGameMode::BeginBetweenRounds()
{
	AColossusGameState* ColossusGameState = GetColossusGameState();
	if (!ColossusGameState)
	{
		return false;
	}
	if (ColossusGameState->GetCurrentRound() >= ColossusGameState->GetTotalRounds())
	{
		return false;
	}

	ColossusGameState->SetRoundState(ColossusGameState->GetCurrentRound(), ColossusGameState->GetTotalRounds(), EColossusRoundPhase::BetweenRounds);
	
	return true;
}

void AColossusGameMode::CompleteMatch()
{
	AColossusGameState* ColossusGameState = GetColossusGameState();
	if (!ColossusGameState)
	{
		return;
	}

	ColossusGameState->SetRoundState(ColossusGameState->GetCurrentRound(), ColossusGameState->GetTotalRounds(), EColossusRoundPhase::Complete);
}

AColossusGameState* AColossusGameMode::GetColossusGameState() const
{
	return Cast<AColossusGameState>(GameState);
}