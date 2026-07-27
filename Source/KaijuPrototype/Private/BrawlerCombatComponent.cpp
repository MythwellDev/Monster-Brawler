#include "BrawlerCombatComponent.h"
#include "BrawlerCharacter.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UBrawlerCombatComponent::UBrawlerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBrawlerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerBrawler = Cast<ABrawlerCharacter>(GetOwner());
}

void UBrawlerCombatComponent::LightAttack()
{
	if (!OwnerBrawler)
	{
		return;
	}

	if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Attacking)
	{
		if (bCanQueueNextAttack)
		{
			bHasQueuedAttack = true;
		}

		return;
	}

	StartAttackFromSet(LightAttacks, 0);
}

void UBrawlerCombatComponent::HeavyAttack()
{
	if (!OwnerBrawler)
	{
		return;
	}

	if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Attacking)
	{
		return;
	}

	StartAttackFromSet(HeavyAttacks, 0);
}

void UBrawlerCombatComponent::StartAttackFromSet(const TArray<FBrawlerAttackData>& AttackSet, int32 AttackIndex)
{
	if (!OwnerBrawler || !OwnerBrawler->CanAttack())
	{
		return;
	}

	if (!AttackSet.IsValidIndex(AttackIndex))
	{
		return;
	}

	CurrentAttackSet = &AttackSet;
	CurrentComboIndex = AttackIndex;
	CurrentAttack = AttackSet[AttackIndex];

	if (!CurrentAttack.Montage)
	{
		return;
	}

	HitActors.Empty();

	bCanQueueNextAttack = false;
	bHasQueuedAttack = false;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Attacking);
	PlayAttackMontage(CurrentAttack);
}

void UBrawlerCombatComponent::OpenComboWindow()
{
	bCanQueueNextAttack = true;
}

void UBrawlerCombatComponent::CloseComboWindow()
{
	bCanQueueNextAttack = false;
}

void UBrawlerCombatComponent::ResetCombo()
{
	CurrentComboIndex = 0;
	CurrentAttackSet = nullptr;
	bCanQueueNextAttack = false;
	bHasQueuedAttack = false;
}

void UBrawlerCombatComponent::PlayAttackMontage(const FBrawlerAttackData& AttackData)
{
	if (!OwnerBrawler)
	{
		return;
	}

	UAnimInstance* AnimInstance = OwnerBrawler->GetMesh()
		? OwnerBrawler->GetMesh()->GetAnimInstance()
		: nullptr;

	if (!AnimInstance || !AttackData.Montage)
	{
		return;
	}

	const float Duration = AnimInstance->Montage_Play(AttackData.Montage);

	if (Duration <= 0.f)
	{
		ResetCombo();
		OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UBrawlerCombatComponent::OnAttackMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackData.Montage);
}

void UBrawlerCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerBrawler)
	{
		return;
	}

	if (bInterrupted)
	{
		ResetCombo();

		if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Attacking)
		{
			OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
		}

		return;
	}

	if (bHasQueuedAttack && CurrentAttackSet)
	{
		bHasQueuedAttack = false;
		bCanQueueNextAttack = false;

		CurrentComboIndex++;

		if (CurrentAttackSet->IsValidIndex(CurrentComboIndex))
		{
			CurrentAttack = (*CurrentAttackSet)[CurrentComboIndex];

			if (CurrentAttack.Montage)
			{
				HitActors.Empty();
				PlayAttackMontage(CurrentAttack);
				return;
			}
		}
	}

	ResetCombo();

	if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Attacking)
	{
		OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
	}
}

void UBrawlerCombatComponent::PerformHitTrace()
{
	if (!OwnerBrawler || !OwnerBrawler->IsAlive())
	{
		return;
	}

	const FVector Start = OwnerBrawler->GetMesh()->GetSocketLocation(CurrentAttack.TraceStartSocket);
	const FVector End = OwnerBrawler->GetMesh()->GetSocketLocation(CurrentAttack.TraceEndSocket);

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	const bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(CurrentAttack.TraceRadius),
		Params
	);

	DrawDebugSphere(GetWorld(), End, CurrentAttack.TraceRadius, 16, FColor::Red, false, 1.f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 3.f);

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor || HitActor == OwnerBrawler || HitActors.Contains(HitActor))
		{
			continue;
		}

		ABrawlerCharacter* HitBrawler = Cast<ABrawlerCharacter>(HitActor);

		if (HitBrawler && HitBrawler->IsAlive())
		{
			HitActors.Add(HitActor);

			HitBrawler->ReceiveDamage(
				CurrentAttack.Damage,
				OwnerBrawler,
				CurrentAttack.Knockback,
				CurrentAttack.HitReactionType
			);

			PlayAttackCameraShake();
			PlayHitStop(CurrentAttack.ImpactFeedback.HitStopDuration);

			break;
		}
	}
}

void UBrawlerCombatComponent::PlayHitReaction(EHitReactionType ReactionType)
{
	if (!OwnerBrawler)
	{
		return;
	}

	UAnimInstance* AnimInstance = OwnerBrawler->GetMesh()
		? OwnerBrawler->GetMesh()->GetAnimInstance()
		: nullptr;

	if (!AnimInstance)
	{
		return;
	}

	for (const FBrawlerHitReactData& HitReact : HitReactions)
	{
		if (HitReact.HitReactionType == ReactionType && HitReact.Montage)
		{
			const float Duration = AnimInstance->Montage_Play(HitReact.Montage);

			if (Duration > 0.f)
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &UBrawlerCombatComponent::OnHitReactionMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, HitReact.Montage);
			}

			return;
		}
	}
}

void UBrawlerCombatComponent::OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerBrawler)
	{
		return;
	}

	if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Stunned)
	{
		OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
	}
}

void UBrawlerCombatComponent::StartBlock()
{
	if (!OwnerBrawler || !OwnerBrawler->IsAlive() || bIsBlocking)
	{
		return;
	}

	if (OwnerBrawler->GetBrawlerState() != EBrawlerState::Idle)
	{
		return;
	}

	bIsBlocking = true;
	OwnerBrawler->SetBrawlerState(EBrawlerState::Blocking);

	if (BlockMontage)
	{
		UAnimInstance* AnimInstance = OwnerBrawler->GetMesh()->GetAnimInstance();

		if (AnimInstance)
		{
			AnimInstance->Montage_Play(BlockMontage);

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &UBrawlerCombatComponent::OnBlockMontageEnded);

			AnimInstance->Montage_SetEndDelegate(EndDelegate, BlockMontage);
		}
	}
}

void UBrawlerCombatComponent::StopBlock()
{
	bIsBlocking = false;

	if (!OwnerBrawler)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = OwnerBrawler->GetMesh()->GetAnimInstance())
	{
		if (BlockMontage && AnimInstance->Montage_IsPlaying(BlockMontage))
		{
			AnimInstance->Montage_JumpToSection("EndBlock", BlockMontage);
			return;
		}
	}

	if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Blocking)
	{
		OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
	}
}

void UBrawlerCombatComponent::OnBlockMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!OwnerBrawler)
	{
		return;
	}

	bIsBlocking = false;

	if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Blocking)
	{
		OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
	}
}

void UBrawlerCombatComponent::PlayAttackCameraShake()
{
	if (!OwnerBrawler || !CurrentAttack.ImpactFeedback.CameraShake)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(OwnerBrawler->GetController());

	if (!PlayerController)
	{
		return;
	}

	PlayerController->ClientStartCameraShake(CurrentAttack.ImpactFeedback.CameraShake);
}

void UBrawlerCombatComponent::PlayHitStop(float Duration)
{
	if (Duration <= 0.f || !GetWorld())
	{
		return;
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			if (GetWorld())
			{
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
			}
		},
		Duration,
		false
	);
}