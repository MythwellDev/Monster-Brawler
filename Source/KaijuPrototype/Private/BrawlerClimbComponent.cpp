


#include "BrawlerClimbComponent.h"
#include "BrawlerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraShakeBase.h"


// Sets default values for this component's properties
UBrawlerClimbComponent::UBrawlerClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBrawlerClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerBrawler = Cast<ABrawlerCharacter>(GetOwner());
}


void UBrawlerClimbComponent::TryStartClimb()
{
	if (!OwnerBrawler) return;

	FHitResult Hit;

	FVector Start = OwnerBrawler->GetActorLocation();
	FVector End = Start + OwnerBrawler->GetActorForwardVector() * ClimbTraceDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f);

	if (!bHit) return;
    if (!Hit.GetActor()->ActorHasTag("Climbable")) return;
	
	ClimbGroundZ = OwnerBrawler->GetActorLocation().Z;
	bIsClimbing = true;

	CurrentClimbActor = Hit.GetActor();
	CurrentWallNormal = Hit.ImpactNormal;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Climbing);

	OwnerBrawler->GetCharacterMovement()->StopMovementImmediately();
	OwnerBrawler->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	OwnerBrawler->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	UE_LOG(LogTemp, Warning, TEXT("Movement Mode After Set: %d"),
		(int32)OwnerBrawler->GetCharacterMovement()->MovementMode);

	// 1. Face the wall FIRST
	FRotator FaceWallRotation = (-Hit.ImpactNormal).Rotation();
	FaceWallRotation.Pitch = 0.f;
	FaceWallRotation.Roll = 0.f;
	OwnerBrawler->SetActorRotation(FaceWallRotation);

	// 2. Now get socket location AFTER rotation
	const FVector ContactSocketLocation =
		OwnerBrawler->GetMesh()->GetSocketLocation(ClimbContactSocketName);

	const FVector SocketToActorOffset =
		OwnerBrawler->GetActorLocation() - ContactSocketLocation;

	// 3. Move actor so the contact socket sits near the wall hit point
	const FVector ClimbLocation =
		Hit.ImpactPoint + Hit.ImpactNormal * WallOffset + SocketToActorOffset;

	OwnerBrawler->SetActorLocation(ClimbLocation);

	UE_LOG(LogTemp, Warning, TEXT("Started Climbing"));
}

bool UBrawlerClimbComponent::IsAtClimbGround() const
{
	if (!OwnerBrawler)
	{
		return false;
	}

	const float CurrentZ = OwnerBrawler->GetActorLocation().Z;

	if (CurrentZ <= ClimbGroundZ + 10.f)
	{
		return true;
	}

	return IsNearGround();
}
void UBrawlerClimbComponent::ResetClimbInput()
{
	CurrentClimbInput = 0.f;
}

void UBrawlerClimbComponent::StopClimb()
{
	if (!OwnerBrawler) return;

	bIsClimbing = false;
	CurrentClimbInput = 0.f;
	CurrentClimbActor = nullptr;
	CurrentWallNormal = FVector::ZeroVector;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
	OwnerBrawler->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	UE_LOG(LogTemp, Warning, TEXT("Stopped Climbing"));
}

void UBrawlerClimbComponent::ClimbMove(float Value)
{
	if (!bIsClimbing || !OwnerBrawler) return;

	CurrentClimbInput = Value;

	if (!IsWallStillClimbable())
	{
		if (!TryMantleToTop())
		{
			StopClimb();
		}

		return;
	}

	if (Value < 0.f && IsAtClimbGround())
	{
		StopClimb();
		return;
	}
	FVector Delta = FVector::UpVector * Value * ClimbSpeed * GetWorld()->GetDeltaSeconds();
	OwnerBrawler->AddActorWorldOffset(Delta, false);
}

bool UBrawlerClimbComponent::IsWallStillClimbable() const
{
	if (!OwnerBrawler)
	{
		return false;
	}

	const FVector Start =
		OwnerBrawler->GetMesh()->GetSocketLocation(ClimbContactSocketName);

	const FVector End =
		Start + (-CurrentWallNormal) * ClimbTraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 0.1f, 0, 5.f);

	return bHit && Hit.GetActor() && Hit.GetActor()->ActorHasTag("Climbable");
}

bool UBrawlerClimbComponent::IsNearGround() const
{
	if (!OwnerBrawler)
	{
		return false;
	}

	const UCapsuleComponent* Capsule = OwnerBrawler->GetCapsuleComponent();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	const FVector Start =
		OwnerBrawler->GetActorLocation() - FVector(0.f, 0.f, HalfHeight - 10.f);

	const FVector End =
		Start - FVector(0.f, 0.f, 40.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.1f, 0, 4.f);

	return bHit && Hit.GetActor() && !Hit.GetActor()->ActorHasTag("Climbable");
}

bool UBrawlerClimbComponent::TryMantleToTop()
{
	if (!OwnerBrawler)
	{
		return false;
	}

	const FVector TowardRoof = -CurrentWallNormal;

	const FVector Start =
		OwnerBrawler->GetActorLocation()
		+ FVector(0.f, 0.f, 150.f)
		+ TowardRoof * 350.f;

	const FVector End =
		Start - FVector(0.f, 0.f, 300.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerBrawler);

	DrawDebugLine(GetWorld(), Start, End, FColor::Purple, false, 2.f, 0, 6.f);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		return false;
	}

	StartMantle(Hit.Location + FVector(0.f, 0.f, 150.f));
	return true;
}

void UBrawlerClimbComponent::StartMantle(const FVector& TargetLocation)
{
	if (!OwnerBrawler)
	{
		return;
	}

	MantleTargetLocation = TargetLocation;

	bIsClimbing = false;
	CurrentClimbInput = 0.f;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Mantling);
	OwnerBrawler->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	OwnerBrawler->GetCharacterMovement()->StopMovementImmediately();

	if (UAnimInstance* AnimInstance = OwnerBrawler->GetMesh()->GetAnimInstance())
	{
		if (MantleMontage)
		{
			AnimInstance->Montage_Play(MantleMontage);
			return;
		}
	}

	FinishMantle();
}

void UBrawlerClimbComponent::FinishMantle()
{
	if (!OwnerBrawler)
	{
		return;
	}

	OwnerBrawler->SetActorLocation(MantleTargetLocation);

	PlayMantleCameraShake();

	CurrentClimbActor = nullptr;
	CurrentWallNormal = FVector::ZeroVector;
	CurrentClimbInput = 0.f;

	OwnerBrawler->SetBrawlerState(EBrawlerState::Idle);
	OwnerBrawler->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	UE_LOG(LogTemp, Warning, TEXT("Finished Mantle"));
}

void UBrawlerClimbComponent::PlayMantleCameraShake()
{
	if (!OwnerBrawler || !MantleCameraShake)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerBrawler->GetController());

	if (!PC)
	{
		return;
	}

	PC->ClientStartCameraShake(MantleCameraShake);
}