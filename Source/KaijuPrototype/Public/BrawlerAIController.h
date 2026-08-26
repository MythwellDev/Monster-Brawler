#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BrawlerAIController.generated.h"

class ABrawlerCharacter;

UCLASS()
class KAIJUPROTOTYPE_API ABrawlerAIController
    : public AAIController
{
    GENERATED_BODY()

public:
    ABrawlerAIController();

protected:
    virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY()
    ABrawlerCharacter* ControlledBrawler = nullptr;

    UPROPERTY()
    AActor* TargetActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float FacingRotationSpeed = 360.f;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float AttackFacingTolerance = 20.f;

	void FaceTarget();

    bool IsFacingTarget() const;

    /*=====================================
                    Movement
    =====================================*/

    UPROPERTY(EditDefaultsOnly, Category = "AI|Movement")
    float MoveAcceptanceRadius = 150.f;

    /*=====================================
                    Combat
    =====================================*/

    // Maximum range at which the AI will attempt any attack.
    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float MaximumAttackRange = 400.f;

    // At this distance, the AI may choose either attack.
    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float CloseAttackRange = 250.f;

    // Chance to use a heavy attack while within close range.
    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CloseRangeHeavyChance = 0.35f;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float MinimumAttackCooldown = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float MaximumAttackCooldown = 1.8f;

    UPROPERTY(EditDefaultsOnly, Category = "AI|Combat")
    float HeavyAttackCooldown = 4.5f;

    bool bCanUseHeavyAttack = true;
    bool bLastAttackWasHeavy = false;

    FTimerHandle HeavyAttackCooldownTimerHandle;

    void ResetHeavyAttackCooldown();

    /*=====================================
                    Decisions
    =====================================*/

    UPROPERTY(EditDefaultsOnly, Category = "AI|Decision")
    float DecisionInterval = 0.2f;

    bool bCanAttack = true;

    FTimerHandle DecisionTimerHandle;
    FTimerHandle AttackCooldownTimerHandle;

    void RunDecision();
    void FindTarget();
    void MoveToTarget();
    void TryAttack(float DistanceToTarget);
    void ResetAttackCooldown();

    bool HasValidTarget() const;
    void StartAttackCooldown();
};