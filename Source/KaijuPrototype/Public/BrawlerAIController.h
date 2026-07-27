// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BrawlerAIController.generated.h"

class ABrawlerCharacter;

/**
 * 
 */
UCLASS()
class KAIJUPROTOTYPE_API ABrawlerAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	ABrawlerAIController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ABrawlerCharacter* ControlledBrawler = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackRange = 350.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MoveAcceptanceRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float DecisionInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackCooldown = 1.2f;

	bool bCanAttack = true;

	FTimerHandle DecisionTimerHandle;
	FTimerHandle AttackCooldownTimerHandle;

	void RunDecision();
	void FindTarget();
	void MoveToTarget();
	void TryAttack();
	void ResetAttackCooldown();
};
