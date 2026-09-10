// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "ColossusFighterStart.generated.h"

/**
 * 
 */
UCLASS()
class KAIJUPROTOTYPE_API AColossusFighterStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	AColossusFighterStart(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Spawning", meta = (ClampMin = "0", UIMin = "0"))
	int32 SpawnIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Spawning")
	FName SpawnGroup = TEXT("Default");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Colossus|Spawning")
	bool bEnabled = true;
};
