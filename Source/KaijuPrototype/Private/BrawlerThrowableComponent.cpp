#include "BrawlerThrowableComponent.h"

#include "BrawlerCharacter.h"
#include "Components/PrimitiveComponent.h"

UBrawlerThrowableComponent::UBrawlerThrowableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBrawlerThrowableComponent::BeginPlay()
{
	Super::BeginPlay();

	ThrowablePrimitive =
		Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	if (ThrowablePrimitive)
	{
		ThrowablePrimitive->SetNotifyRigidBodyCollision(true);

		ThrowablePrimitive->OnComponentHit.AddUniqueDynamic(
			this,
			&UBrawlerThrowableComponent::HandleThrowableHit
		);
	}
}

void UBrawlerThrowableComponent::BeginThrow(ABrawlerCharacter* InThrower)
{
	Thrower = InThrower;
	bIsInFlight = true;
	bHasDealtImpactDamage = false;
	bCanBePickedUp = false;
}

void UBrawlerThrowableComponent::HandleThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsInFlight ||
		bHasDealtImpactDamage ||
		!OtherActor ||
		OtherActor == GetOwner())
	{
		return;
	}

	// Prevent the pipe from damaging its thrower upon release.
	if (OtherActor == Thrower)
	{
		return;
	}

	ABrawlerCharacter* HitBrawler =
		Cast<ABrawlerCharacter>(OtherActor);

	if (HitBrawler && HitBrawler->IsAlive())
	{
		bHasDealtImpactDamage = true;

		HitBrawler->ReceiveDamage(
			ImpactDamage,
			Thrower,
			ImpactKnockback,
			ImpactReactionType
		);
	}

	// The first valid collision ends this throw.
	bIsInFlight = false;
	bCanBePickedUp = true;
}