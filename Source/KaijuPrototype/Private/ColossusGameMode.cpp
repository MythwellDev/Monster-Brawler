// Fill out your copyright notice in the Description page of Project Settings.


#include "ColossusGameMode.h"
#include "ColossusFighterStart.h"
#include "EngineUtils.h"
#include "Engine/World.h"


AColossusGameMode::AColossusGameMode()
{
	GameStateClass = AColossusGameState::StaticClass();
}

AActor* AColossusGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (!Player) return nullptr;
	AColossusFighterStart* FighterStart = FindFighterStart(0, FName(TEXT("Default")));
	
	if (FighterStart)
	{
		return FighterStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

AColossusFighterStart* AColossusGameMode::FindFighterStart(int32 SpawnIndex, FName SpawnGroup) const
{
	if (!GetWorld()) return nullptr;

	for (TActorIterator<AColossusFighterStart> It(GetWorld()); It; ++It)
	{
		AColossusFighterStart* FighterStart = *It;
		if (FighterStart && FighterStart->bEnabled && FighterStart->SpawnIndex == SpawnIndex && FighterStart->SpawnGroup == SpawnGroup)
		{
			return FighterStart;
		}
	}

	return nullptr;
}

ABrawlerCharacter* AColossusGameMode::SpawnCPUFighterAtIndex(TSubclassOf<ABrawlerCharacter> FighterClass, int32 SpawnIndex, FName SpawnGroup)
{
	if (!FighterClass || !GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCPUFighterAtIndex: Invalid FighterClass or World"));
		return nullptr;
	}

	AColossusFighterStart* FighterStart = FindFighterStart(SpawnIndex, SpawnGroup);
	
	if (!FighterStart)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCPUFighterAtIndex: Invalid FighterStart"));
		return nullptr;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABrawlerCharacter* SpawnedFighter = GetWorld()->SpawnActor<ABrawlerCharacter>(FighterClass, FighterStart->GetActorTransform(), SpawnParameters);

	if (!SpawnedFighter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnCPUFighterAtIndex: Failed to spawn fighter"));
		return nullptr;
	}

	if (!SpawnedFighter->GetController())
	{
		SpawnedFighter->SpawnDefaultController();
	}

	return SpawnedFighter;
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

bool AColossusGameMode::GetCurrentRoundDefinition(FColossusRoundDefinition& OutRoundDefinition) const
{
	const AColossusGameState* ColossusGameState = GetColossusGameState();

	if (!ColossusGameState)
	{
		return false;
	}
	
	const int32 DefinitionIndex = ColossusGameState->GetCurrentRound() - 1;

	if (!RoundDefinitions.IsValidIndex(DefinitionIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCurrentRoundDefinition: Invalid round index %d"), ColossusGameState->GetCurrentRound());
		return false;
	}

	OutRoundDefinition = RoundDefinitions[DefinitionIndex];

	if (!OutRoundDefinition.FighterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetCurrentRoundDefinition: Invalid FighterClass in round definition %d"), ColossusGameState->GetCurrentRound());
		return false;
	}

	return true;
}