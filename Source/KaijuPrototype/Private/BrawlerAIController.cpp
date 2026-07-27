// Fill out your copyright notice in the Description page of Project Settings.


#include "BrawlerAIController.h"
#include "BrawlerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "BrawlerTargetingComponent.h"

ABrawlerAIController::ABrawlerAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABrawlerAIController::BeginPlay()
{
	Super::BeginPlay();

	ControlledBrawler = Cast<ABrawlerCharacter>(GetPawn());
	FindTarget();

	GetWorldTimerManager().SetTimer(
		DecisionTimerHandle,
		this,
		&ABrawlerAIController::RunDecision,
		DecisionInterval,
		true
	);
}

void ABrawlerAIController::FindTarget()
{
	TargetActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ABrawlerAIController::RunDecision()
{
	if (!ControlledBrawler || !ControlledBrawler->IsAlive())
	{
		return;
	}

	if (!TargetActor)
	{
		FindTarget();
		return;
	}

	const float Distance = FVector::Dist(
		ControlledBrawler->GetActorLocation(),
		TargetActor->GetActorLocation()
	);

	if (Distance > AttackRange)
	{
		MoveToTarget();
	}
	else
	{
		StopMovement();
		TryAttack();
	}
}

void ABrawlerAIController::MoveToTarget()
{
	if (!TargetActor)
	{
		return;
	}

	MoveToActor(TargetActor, MoveAcceptanceRadius);
}

void ABrawlerAIController::TryAttack()
{
	if (!ControlledBrawler || !bCanAttack)
	{
		return;
	}

	if (!ControlledBrawler->CanAttack()) return;

	UE_LOG(LogTemp, Warning, TEXT("AI TryAttack. State: %s CanAttack: %s"),
		*UEnum::GetValueAsString(ControlledBrawler->GetBrawlerState()),
		ControlledBrawler->CanAttack() ? TEXT("true") : TEXT("false"));

	ControlledBrawler->LightAttack();

	bCanAttack = false;

	GetWorldTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&ABrawlerAIController::ResetAttackCooldown,
		AttackCooldown,
		false
	);
}

void ABrawlerAIController::ResetAttackCooldown()
{
	bCanAttack = true;
}