// Fill out your copyright notice in the Description page of Project Settings.


#include "ColossusGameState.h"
#include "Net/UnrealNetwork.h"

AColossusGameState::AColossusGameState()
{
	CurrentRound = 0;
	TotalRounds = 2;
	RoundPhase = EColossusRoundPhase::Waiting;
}

void AColossusGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AColossusGameState, CurrentRound);
	DOREPLIFETIME(AColossusGameState, TotalRounds);
	DOREPLIFETIME(AColossusGameState, RoundPhase);
}

void AColossusGameState::SetRoundState(int32 NewCurrentRound, int32 NewTotalRounds, EColossusRoundPhase NewRoundPhase)
{
	if (!HasAuthority()) return;

	NewCurrentRound = FMath::Max(0, NewCurrentRound);
	NewTotalRounds = FMath::Max(1, NewTotalRounds);

	if (CurrentRound == NewCurrentRound && TotalRounds == NewTotalRounds && RoundPhase == NewRoundPhase)
	{
		return;
	}

	CurrentRound = NewCurrentRound;
	TotalRounds = NewTotalRounds;
	RoundPhase = NewRoundPhase;

	BroadcastRoundState();
}

void AColossusGameState::OnRep_RoundState()
{
	BroadcastRoundState();
}

void AColossusGameState::BroadcastRoundState()
{
	OnRoundStateChanged.Broadcast(CurrentRound, TotalRounds, RoundPhase);
}
