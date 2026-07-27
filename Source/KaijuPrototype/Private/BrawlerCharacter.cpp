#include "BrawlerCharacter.h"
#include "BrawlerCombatComponent.h"
#include "BrawlerHealthComponent.h"
#include "BrawlerGrabComponent.h"
#include "BrawlerClimbComponent.h"
#include "BrawlerTargetingComponent.h"
#include "BrawlerThrowableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABrawlerCharacter::ABrawlerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CombatComponent = CreateDefaultSubobject<UBrawlerCombatComponent>(TEXT("CombatComponent"));
	HealthComponent = CreateDefaultSubobject<UBrawlerHealthComponent>(TEXT("HealthComponent"));
	GrabComponent = CreateDefaultSubobject<UBrawlerGrabComponent>(TEXT("GrabComponent"));
	ClimbComponent = CreateDefaultSubobject<UBrawlerClimbComponent>(TEXT("ClimbComponent"));
	TargetingComponent = CreateDefaultSubobject<UBrawlerTargetingComponent>(TEXT("TargetingComponent"));

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
}

void ABrawlerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = WalkSpeed;
	}

	SetBrawlerState(EBrawlerState::Idle);
}

//State Accessors
void ABrawlerCharacter::SetBrawlerState(EBrawlerState NewState)
{
	CurrentState = NewState;
}

EBrawlerState ABrawlerCharacter::GetBrawlerState() const
{
	return CurrentState;
}

bool ABrawlerCharacter::CanAttack() const
{
	return IsAlive() && CurrentState == EBrawlerState::Idle;
}

bool ABrawlerCharacter::IsAlive() const
{
	return HealthComponent && HealthComponent->IsAlive() && CurrentState != EBrawlerState::Dead;
}


//Combat Component Accessors
void ABrawlerCharacter::LightAttack()
{
	if (CombatComponent)
	{
		CombatComponent->LightAttack();
	}
}

void ABrawlerCharacter::HeavyAttack()
{
	if (CombatComponent)
	{
		CombatComponent->HeavyAttack();
	}
}

void ABrawlerCharacter::PerformHitTrace()
{
	if (CombatComponent)
	{
		CombatComponent->PerformHitTrace();
	}
}

//Health Component Accessors
void ABrawlerCharacter::ReceiveDamage(float DamageAmount, ABrawlerCharacter* Attacker, float KnockbackAmount, EHitReactionType HitReactionType)
{
	if (HealthComponent)
	{
		HealthComponent->ReceiveDamage(DamageAmount, Attacker, KnockbackAmount, HitReactionType);
	}
}


//Grab Component Accessors
void ABrawlerCharacter::TryGrab()
{
	if (GrabComponent)
	{
		GrabComponent->TryGrab();
	}
}

void ABrawlerCharacter::ThrowGrabbedTarget()
{
	if (GrabComponent)
	{
		GrabComponent->ThrowGrabbedTarget();
	}
}

bool ABrawlerCharacter::IsGrabbing() const
{
	return GrabComponent && GrabComponent->IsGrabbing();
}