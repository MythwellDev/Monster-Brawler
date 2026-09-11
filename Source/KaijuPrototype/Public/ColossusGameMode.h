// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ColossusGameState.h"
#include "BrawlerCharacter.h"
#include "ColossusMatchTypes.h"
#include "ColossusGameMode.generated.h"

class AColossusFighterStart;

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

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Colossus|Spawning")
	ABrawlerCharacter* SpawnCPUFighterAtIndex(TSubclassOf<ABrawlerCharacter> FighterClass, int32 SpawnIndex, FName SpawnGroup);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Colossus|Match")
	bool GetCurrentRoundDefinition(FColossusRoundDefinition& OutRoundDefinition) const;

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colossus|Match")
	TArray<FColossusRoundDefinition> RoundDefinitions;

private:
	AColossusGameState* GetColossusGameState() const;
	AColossusFighterStart* FindFighterStart(int32 SpawnIndex, FName SpawnGroup) const;
};
