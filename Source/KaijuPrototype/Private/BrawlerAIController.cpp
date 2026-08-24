#include "BrawlerAIController.h"

#include "BrawlerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


ABrawlerAIController::ABrawlerAIController()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABrawlerAIController::BeginPlay()
{
    Super::BeginPlay();

    ControlledBrawler = Cast<ABrawlerCharacter>(GetPawn());

    FindTarget();

    GetWorldTimerManager().SetTimer(DecisionTimerHandle, this, &ABrawlerAIController::RunDecision, DecisionInterval, true);
}

void ABrawlerAIController::FaceTarget()
{
    if (!ControlledBrawler || !TargetActor)
    {
        return;
    }

    FVector DirectionToTarget =
        TargetActor->GetActorLocation() - ControlledBrawler->GetActorLocation();

    DirectionToTarget.Z = 0.f;

    if (DirectionToTarget.IsNearlyZero())
    {
        return;
    }

    const FRotator DesiredRotation = DirectionToTarget.Rotation();
    const FRotator NewRotation = FMath::RInterpConstantTo(
        ControlledBrawler->GetActorRotation(),
        DesiredRotation,
        DecisionInterval,
        FacingRotationSpeed);

    ControlledBrawler->SetActorRotation(NewRotation);
}

bool ABrawlerAIController::IsFacingTarget() const
{
    if (!ControlledBrawler || !TargetActor)
    {
        return false;
    }

    FVector DirectionToTarget =
        TargetActor->GetActorLocation() - ControlledBrawler->GetActorLocation();

    DirectionToTarget.Z = 0.f;
    DirectionToTarget.Normalize();

    const FVector Forward = ControlledBrawler->GetActorForwardVector();
    const float FacingDot = FVector::DotProduct(Forward, DirectionToTarget);
    const float MinimumFacingDot = FMath::Cos(FMath::DegreesToRadians(AttackFacingTolerance));

    return FacingDot >= MinimumFacingDot;
}

void ABrawlerAIController::FindTarget()
{
    TargetActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (TargetActor)
    {
        SetFocus(TargetActor);
    }
}

bool ABrawlerAIController::HasValidTarget() const
{
    if (!IsValid(TargetActor))
    {
        return false;
    }

    const ABrawlerCharacter* TargetBrawler = Cast<ABrawlerCharacter>(TargetActor);

    return !TargetBrawler || TargetBrawler->IsAlive();
}

void ABrawlerAIController::RunDecision()
{
    if (!ControlledBrawler || !ControlledBrawler->IsAlive())
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Gameplay);
        return;
    }

    if (!HasValidTarget())
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Gameplay);
        FindTarget();
        return;
    }

    // Do not issue movement or attack commands while another
    // gameplay state owns the character.
    if (ControlledBrawler->GetBrawlerState() != EBrawlerState::Idle)
    {
        StopMovement();
        return;
    }

    SetFocus(TargetActor);

    const float DistanceToTarget = FVector::Dist2D(ControlledBrawler->GetActorLocation(), TargetActor->GetActorLocation());

    if (DistanceToTarget > MaximumAttackRange)
    {
        MoveToTarget();
        return;
    }

    StopMovement();
    FaceTarget();

    if (!IsFacingTarget())
    {
        return;
    }

    TryAttack(DistanceToTarget);
}

void ABrawlerAIController::MoveToTarget()
{
    if (!HasValidTarget())
    {
        return;
    }

    MoveToActor(TargetActor, MoveAcceptanceRadius);
}

void ABrawlerAIController::TryAttack(float DistanceToTarget)
{
    if (!ControlledBrawler || !bCanAttack || !ControlledBrawler->CanAttack())
    {
        return;
    }

    bool bUseHeavyAttack = false;

    if (DistanceToTarget > CloseAttackRange)
    {
        // At outer range, only use the heavy attack when it
        // is available and was not the previous attack.
        if (bCanUseHeavyAttack && !bLastAttackWasHeavy)
        {
            bUseHeavyAttack = true;
        }
        else
        {
            // Move closer so the AI can use its light attack
            // instead of waiting or spamming heavy attacks.
            MoveToTarget();
            return;
        }
    }
    else
    {
        bUseHeavyAttack = bCanUseHeavyAttack && !bLastAttackWasHeavy && FMath::FRand() <= CloseRangeHeavyChance;
    }

    if (bUseHeavyAttack)
    {
        ControlledBrawler->HeavyAttack();

        bLastAttackWasHeavy = true;
        bCanUseHeavyAttack = false;

        GetWorldTimerManager().SetTimer(HeavyAttackCooldownTimerHandle, this, &ABrawlerAIController::ResetHeavyAttackCooldown, HeavyAttackCooldown, false);
    }
    else
    {
        ControlledBrawler->LightAttack();
        bLastAttackWasHeavy = false;
    }

    StartAttackCooldown();
}
void ABrawlerAIController::StartAttackCooldown()
{
    bCanAttack = false;

    const float CooldownDuration = FMath::FRandRange(MinimumAttackCooldown, MaximumAttackCooldown);

    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &ABrawlerAIController::ResetAttackCooldown, CooldownDuration, false);
}

void ABrawlerAIController::ResetAttackCooldown()
{
    bCanAttack = true;
}

void ABrawlerAIController::ResetHeavyAttackCooldown()
{
    bCanUseHeavyAttack = true;
}