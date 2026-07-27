#include "BrawlerGrabComponent.h"
#include "BrawlerCharacter.h"
#include "BrawlerThrowableComponent.h"

#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UBrawlerGrabComponent::UBrawlerGrabComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBrawlerGrabComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerBrawler = Cast<ABrawlerCharacter>(GetOwner());
}

void UBrawlerGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateThrowableCandidate();
}

/*========================================
	Main Input
========================================*/

void UBrawlerGrabComponent::GrabOrThrow()
{
	if (GrabbedTarget || HeldThrowable)
	{
		OwnerBrawler->SetBrawlerState(EBrawlerState::Throwing);

		if (ThrowMontage)
		{
			OwnerBrawler->PlayAnimMontage(ThrowMontage);
			return;
		}

		ConfirmThrow();
		return;
	}

	TryGrab();

	if (!PendingGrabTarget && !GrabbedTarget)
	{
		TryGrabThrowable();
	}
}

void UBrawlerGrabComponent::ConfirmThrow()
{
	if (GrabbedTarget)
	{
		ThrowGrabbedTarget();
		return;
	}

	if (HeldThrowable)
	{
		ThrowHeldThrowable();
	}
}

bool UBrawlerGrabComponent::IsGrabbing() const
{
	return GrabbedTarget != nullptr || HeldThrowable != nullptr;
}

bool UBrawlerGrabComponent::CanGrab() const
{
	return OwnerBrawler
		&& OwnerBrawler->IsAlive()
		&& OwnerBrawler->GetBrawlerState() == EBrawlerState::Idle
		&& !GrabbedTarget
		&& !HeldThrowable;
}

/*========================================
	Brawler Grab
========================================*/

void UBrawlerGrabComponent::TryGrab()
{
	if (!CanGrab())
	{
		return;
	}

	PendingGrabTarget = FindGrabbableTarget();

	if (!PendingGrabTarget)
	{
		return;
	}

	OwnerBrawler->SetBrawlerState(EBrawlerState::Grabbing);

	if (GrabMontage)
	{
		OwnerBrawler->PlayAnimMontage(GrabMontage);
		return;
	}

	ConfirmGrab();
}

void UBrawlerGrabComponent::ConfirmGrab()
{
	if (!OwnerBrawler)
	{
		return;
	}

	if (PendingGrabTarget)
	{
		AttachTarget(PendingGrabTarget);
		PendingGrabTarget = nullptr;
		return;
	}

	if (PendingThrowable)
	{
		AttachThrowable(PendingThrowable);
		PendingThrowable = nullptr;
	}
}

ABrawlerCharacter* UBrawlerGrabComponent::FindGrabbableTarget() const
{
	if (!OwnerBrawler)
	{
		return nullptr;
	}

	const FVector Start = OwnerBrawler->GetActorLocation() + FVector(0.f, 0.f, 120.f);
	const FVector End = Start + OwnerBrawler->GetActorForwardVector() * GrabRange;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	const bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(GrabRadius),
		Params
	);

	DrawDebugSphere(GetWorld(), End, GrabRadius, 16, FColor::Blue, false, 1.f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.f, 0, 3.f);

	if (!bHit)
	{
		return nullptr;
	}

	for (const FHitResult& Hit : HitResults)
	{
		ABrawlerCharacter* HitBrawler = Cast<ABrawlerCharacter>(Hit.GetActor());

		if (HitBrawler && HitBrawler != OwnerBrawler)
		{
			return HitBrawler;
		}
	}

	return nullptr;
}

void UBrawlerGrabComponent::AttachTarget(ABrawlerCharacter* Target)
{
	if (!OwnerBrawler || !Target)
	{
		return;
	}

	GrabbedTarget = Target;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Grabbing);
	Target->SetBrawlerState(EBrawlerState::Grabbed);

	Target->GetCharacterMovement()->DisableMovement();
	Target->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Target->AttachToComponent(
		OwnerBrawler->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		GrabSocketName
	);

	const FTransform HandSocketTransform =
		OwnerBrawler->GetMesh()->GetSocketTransform(GrabSocketName, RTS_World);

	const FTransform GrabbedSocketTransform =
		Target->GetMesh()->GetSocketTransform(GrabbedSocketName, RTS_World);

	const FVector Offset =
		Target->GetActorLocation() - GrabbedSocketTransform.GetLocation();

	Target->SetActorLocation(HandSocketTransform.GetLocation() + Offset);

	Target->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Target->GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

ABrawlerCharacter* UBrawlerGrabComponent::DetachGrabbedTarget()
{
	if (!GrabbedTarget)
	{
		return nullptr;
	}

	ABrawlerCharacter* Target = GrabbedTarget;
	GrabbedTarget = nullptr;

	Target->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Target->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Target->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	Target->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	Target->GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

	return Target;
}

void UBrawlerGrabComponent::ThrowGrabbedTarget()
{
	if (!OwnerBrawler || !GrabbedTarget)
	{
		return;
	}

	ABrawlerCharacter* Target = GrabbedTarget;
	GrabbedTarget = nullptr;

	const FVector ThrowDirection = GetThrowDirection();

	Target->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	OwnerBrawler->MoveIgnoreActorAdd(Target);
	Target->MoveIgnoreActorAdd(OwnerBrawler);

	Target->SetActorLocation(
		OwnerBrawler->GetActorLocation()
		+ ThrowDirection * 250.f
		+ FVector(0.f, 0.f, 50.f)
	);

	Target->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Target->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	Target->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	Target->GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);

	Target->SetBrawlerState(EBrawlerState::Stunned);
	OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);

	DrawDebugLine(
		GetWorld(),
		OwnerBrawler->GetActorLocation(),
		OwnerBrawler->GetActorLocation() + ThrowDirection * 1000.f,
		FColor::Green,
		false,
		2.f,
		0,
		8.f
	);

	Target->LaunchCharacter(ThrowDirection * ThrowForce, true, true);

	FTimerHandle IgnoreRestoreTimer;

	GetWorld()->GetTimerManager().SetTimer(
		IgnoreRestoreTimer,
		[this, Target]()
		{
			if (!OwnerBrawler || !Target)
			{
				return;
			}

			OwnerBrawler->MoveIgnoreActorRemove(Target);
			Target->MoveIgnoreActorRemove(OwnerBrawler);
		},
		0.3f,
		false
	);
}

void UBrawlerGrabComponent::ReleaseGrabbedTarget()
{
	if (!OwnerBrawler || !GrabbedTarget)
	{
		return;
	}

	ABrawlerCharacter* Target = DetachGrabbedTarget();

	if (!Target)
	{
		return;
	}

	Target->SetBrawlerState(EBrawlerState::Idle);
	OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
}

/*========================================
	Throwable Grab
========================================*/

void UBrawlerGrabComponent::TryGrabThrowable()
{
	if (!CanGrab())
	{
		return;
	}

	PendingThrowable = CurrentThrowableCandidate ? CurrentThrowableCandidate : FindThrowableTarget();

	if (!PendingThrowable)
	{
		return;
	}

	OwnerBrawler->SetBrawlerState(EBrawlerState::Grabbing);

	if (GrabMontage)
	{
		OwnerBrawler->PlayAnimMontage(GrabMontage);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("PendingThrowable: %s"),
		PendingThrowable ? *PendingThrowable->GetName() : TEXT("None"));

	ConfirmGrab();
}

AActor* UBrawlerGrabComponent::FindThrowableTarget() const
{
	if (!OwnerBrawler)
	{
		return nullptr;
	}

	const FVector Start = OwnerBrawler->GetActorLocation() + FVector(0.f, 0.f, 40.f);
	const FVector End = Start + OwnerBrawler->GetActorForwardVector() * GrabRange;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	const bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(GrabRadius),
		Params
	);

	DrawDebugSphere(GetWorld(), End, GrabRadius, 16, FColor::Orange, false, 0.02f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 0.02f, 0, 3.f);

	if (!bHit)
	{
		return nullptr;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor || HitActor == OwnerBrawler)
		{
			continue;
		}

		UBrawlerThrowableComponent* ThrowableComp =
			HitActor->FindComponentByClass<UBrawlerThrowableComponent>();

		if (ThrowableComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Throwable: %s"), *HitActor->GetName());
			return HitActor;
		}
	}

	return nullptr;
}

void UBrawlerGrabComponent::AttachThrowable(AActor* Throwable)
{
	if (!OwnerBrawler || !Throwable)
	{
		return;
	}

	HeldThrowable = Throwable;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Grabbing);

	UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Throwable->GetRootComponent());

	if (RootPrimitive)
	{
		RootPrimitive->SetSimulatePhysics(false);
		RootPrimitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UBrawlerThrowableComponent* ThrowableComp =
		Throwable->FindComponentByClass<UBrawlerThrowableComponent>();

	const FName Socket =
		ThrowableComp ? ThrowableComp->AttachSocket : ThrowableSocketName;

	Throwable->AttachToComponent(
		OwnerBrawler->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		Socket
	);
}

void UBrawlerGrabComponent::ThrowHeldThrowable()
{
	if (!OwnerBrawler || !HeldThrowable)
	{
		return;
	}

	AActor* Throwable = HeldThrowable;
	HeldThrowable = nullptr;

	const FVector ThrowDirection = GetThrowDirection();

	Throwable->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Throwable->GetRootComponent());

	if (RootPrimitive)
	{
		RootPrimitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		RootPrimitive->SetSimulatePhysics(true);
		RootPrimitive->SetEnableGravity(true);

		UBrawlerThrowableComponent* ThrowableComponent =
			Throwable->FindComponentByClass<UBrawlerThrowableComponent>();

		const float ThrowSpeed =
			ThrowableComponent
			? ThrowableComponent->ThrowSpeed
			: ThrowForce;

		RootPrimitive->SetPhysicsLinearVelocity(
			ThrowDirection * ThrowSpeed
		);
	}

	OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);

	DrawDebugLine(
		GetWorld(),
		OwnerBrawler->GetActorLocation(),
		OwnerBrawler->GetActorLocation() + ThrowDirection * 1000.f,
		FColor::Orange,
		false,
		2.f,
		0,
		8.f
	);
}

/*========================================
	Shared
========================================*/

FVector UBrawlerGrabComponent::GetThrowDirection() const
{
	if (!OwnerBrawler)
	{
		return FVector::ForwardVector;
	}

	const FRotator ControlRotation = OwnerBrawler->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	FVector ThrowDirection = YawRotation.Vector();
	ThrowDirection.Z = 0.25f;
	ThrowDirection.Normalize();

	return ThrowDirection;
}

void UBrawlerGrabComponent::UpdateThrowableCandidate()
{
	if (!OwnerBrawler || IsGrabbing() || OwnerBrawler->GetBrawlerState() != EBrawlerState::Idle)
	{
		SetThrowableHighlight(CurrentThrowableCandidate, false);
		CurrentThrowableCandidate = nullptr;
		return;
	}

	AActor* NewCandidate = FindThrowableTarget();

	if (NewCandidate == CurrentThrowableCandidate)
	{
		return;
	}

	SetThrowableHighlight(CurrentThrowableCandidate, false);

	CurrentThrowableCandidate = NewCandidate;

	SetThrowableHighlight(CurrentThrowableCandidate, true);
}

void UBrawlerGrabComponent::SetThrowableHighlight(AActor* Actor, bool bHighlighted)
{
	if (!Actor)
	{
		return;
	}

	UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Actor->GetRootComponent());

	if (Primitive)
	{
		Primitive->SetRenderCustomDepth(bHighlighted);
	}
}