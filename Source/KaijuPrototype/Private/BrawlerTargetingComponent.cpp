
#include "BrawlerTargetingComponent.h"
#include "BrawlerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UBrawlerTargetingComponent::UBrawlerTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBrawlerTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerBrawler = Cast<ABrawlerCharacter>(GetOwner());
}

void UBrawlerTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentTarget && OwnerBrawler)
	{
		ABrawlerCharacter* TargetBrawler = Cast<ABrawlerCharacter>(CurrentTarget);

		const float Distance = FVector::Dist(OwnerBrawler->GetActorLocation(), CurrentTarget->GetActorLocation());

		if (!TargetBrawler || !TargetBrawler->IsAlive() || Distance > TargetSearchRadius)
		{
			StopTargeting();
		}
	}

	UpdateCombatCamera(DeltaTime);
}

void UBrawlerTargetingComponent::ToggleTargeting()
{
	if (CurrentTarget)
	{
		StopTargeting();
		return;
	}

	FindTarget();
}

void UBrawlerTargetingComponent::StopTargeting()
{
	CurrentTarget = nullptr;
}

void UBrawlerTargetingComponent::FindTarget()
{
	if (!OwnerBrawler)
	{
		return;
	}

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABrawlerCharacter::StaticClass(), FoundActors);

	AActor* BestTarget = nullptr;
	float BestDistance = TargetSearchRadius;

	for (AActor* Actor : FoundActors)
	{
		if (!Actor || Actor == OwnerBrawler)
		{
			continue;
		}

		ABrawlerCharacter* Brawler = Cast<ABrawlerCharacter>(Actor);
		if (!Brawler || !Brawler->IsAlive())
		{
			continue;
		}

		const float Distance = FVector::Dist(OwnerBrawler->GetActorLocation(), Brawler->GetActorLocation());

		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			BestTarget = Brawler;
		}
	}

	CurrentTarget = BestTarget;
}

void UBrawlerTargetingComponent::UpdateCombatCamera(float DeltaTime)
{
	if (!OwnerBrawler)
	{
		return;
	}

	USpringArmComponent* SpringArm = OwnerBrawler->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm)
	{
		return;
	}

	const bool bTargeting = CurrentTarget != nullptr;

	const float DesiredArmLength = bTargeting ? TargetingArmLength : DefaultArmLength;
	const FVector DesiredSocketOffset = bTargeting ? TargetingSocketOffset : DefaultSocketOffset;

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, DesiredArmLength, DeltaTime, CameraPresetInterpSpeed);

	SpringArm->SocketOffset = FMath::VInterpTo(SpringArm->SocketOffset, DesiredSocketOffset, DeltaTime, CameraPresetInterpSpeed);
	
	if (!bTargeting)
	{
		return;
	}

	AController* Controller = OwnerBrawler->GetController();
	if (!Controller)
	{
		return;
	}

	const FVector ToTarget = (CurrentTarget->GetActorLocation() - OwnerBrawler->GetActorLocation()).GetSafeNormal();

	FRotator DesiredRotation = ToTarget.Rotation();
	DesiredRotation.Yaw += TargetingYawOffset;
	DesiredRotation.Pitch = TargetingPitch;
	DesiredRotation.Roll = 0.f;

	const FRotator NewRotation = FMath::RInterpTo(Controller->GetControlRotation(), DesiredRotation, DeltaTime, CameraPresetInterpSpeed);

	Controller->SetControlRotation(NewRotation);
}

bool UBrawlerTargetingComponent::GetPlayerAimPoint(FVector& OutAimPoint, const AActor* ActorToIgnore) const
{
	if (!OwnerBrawler)
	{
		return false;
	}
	
	APlayerController* PlayerController = Cast<APlayerController>(OwnerBrawler->GetController());
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return false;
	}
	
	FVector CameraLocation;
	
	FRotator CameraRotation;
	
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	const FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * AimTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerBrawler);
	if (ActorToIgnore)
	{
		QueryParams.AddIgnoredActor(ActorToIgnore);
	}

	FHitResult HitResult;
	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, QueryParams);

	OutAimPoint = bHit ? HitResult.ImpactPoint : TraceEnd;
	
	return true;
}