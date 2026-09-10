// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ColossusGameState.h"
#include "ColossusGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KAIJUPROTOTYPE_API AColossusGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AColossusGameMode();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Colossus|Match")
	void BeginNextRound();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Colossus|Match")
	void ActivateCurrentRound();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Colossus|Match")
	bool BeginBetweenRounds();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Colossus|Match")
	void CompleteMatch();

private:
	AColossusGameState* GetColossusGameState() const;
};
