#include "BrawlerAIController.h"
#include "Navigation/PathFollowingComponent.h"

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
}

void ABrawlerAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledBrawler = Cast<ABrawlerCharacter>(InPawn);
	FindTarget();

	GetWorldTimerManager().ClearTimer(DecisionTimerHandle);
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
    const FRotator NewRotation = FMath::RInterpConstantTo(ControlledBrawler->GetActorRotation(), DesiredRotation, DecisionInterval, FacingRotationSpeed);

    ControlledBrawler->SetActorRotation(NewRotation);
}

bool ABrawlerAIController::IsFacingTarget() const
{
    if (!ControlledBrawler || !TargetActor)
    {
        return false;
    }

    FVector DirectionToTarget = TargetActor->GetActorLocation() - ControlledBrawler->GetActorLocation();

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
        FindTarget();

        if (!HasValidTarget())
        {
            StopMovement();
            ClearFocus(EAIFocusPriority::Gameplay);
            return;
        }
    }

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

    if (!bCanAttack)
    {
        if (DistanceToTarget > CloseAttackRange)
        {
            MoveToTarget();
        }
        else
        {
            StopMovement();
			FaceTarget();
		}

        return;
    }

    if (DistanceToTarget > CloseAttackRange && (!bCanUseHeavyAttack || bLastAttackWasHeavy))
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

    if (GetMoveStatus() == EPathFollowingStatus::Moving)
    {
        return;
	}

	const EPathFollowingRequestResult::Type MoveResult = MoveToActor(TargetActor, MoveAcceptanceRadius, true, true, false, nullptr, false);

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s failed to move toward %s"), *GetNameSafe(ControlledBrawler), *GetNameSafe(TargetActor));
	}

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
		bUseHeavyAttack = bCanUseHeavyAttack && !bLastAttackWasHeavy;
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