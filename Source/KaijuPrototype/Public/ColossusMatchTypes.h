#pragma once

#include "CoreMinimal.h"
#include "ColossusMatchTypes.generated.h"

class ABrawlerCharacter;

USTRUCT(BlueprintType)
struct FColossusRoundDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Round")
	TSubclassOf<ABrawlerCharacter> FighterClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Round",	meta = (ClampMin = "0", UIMin = "0"))
	int32 SpawnIndex = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Round")
	FName SpawnGroup = TEXT("Default");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Round",	meta = (ClampMin = "1.0", UIMin = "1.0"))
	float StartingHealth = 100.0f;
};