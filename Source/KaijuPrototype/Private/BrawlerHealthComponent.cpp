
#include "BrawlerHealthComponent.h"
#include "BrawlerCharacter.h"
#include "BrawlerCombatComponent.h"
#include "BrawlerThrowableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBrawlerHealthComponent::UBrawlerHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBrawlerHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerBrawler = Cast<ABrawlerCharacter>(GetOwner());
	CurrentHealth = MaxHealth;

	if (OwnerBrawler)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s spawned with MaxHealth: %f CurrentHealth: %f"),
			*OwnerBrawler->GetName(),
			MaxHealth,
			CurrentHealth);
	}
}
/*=====================================
			Health Accessors
=====================================*/
bool UBrawlerHealthComponent::IsAlive() const
{
	return CurrentHealth > 0.f;
}


/*=====================================
			Damage Handling
=====================================*/
void UBrawlerHealthComponent::ReceiveDamage(float DamageAmount, ABrawlerCharacter* Attacker, float KnockbackAmount, EHitReactionType HitReactionType)
{
    if (!OwnerBrawler || !IsAlive())
    {
        return;
    }

    float FinalDamage = DamageAmount;

    if (UBrawlerCombatComponent* CombatComp = OwnerBrawler->GetCombatComponent())
    {
        if (CombatComp->IsBlocking())
        {
            FinalDamage *= CombatComp->GetBlockDamageMultiplier();
        }
    }

    CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0.f, MaxHealth);

    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage from %s. Health: %f"),
        *OwnerBrawler->GetName(),
        FinalDamage,
        Attacker ? *Attacker->GetName() : TEXT("Unknown"),
        CurrentHealth);

    if (CurrentHealth <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s DIED"), *OwnerBrawler->GetName());

        OwnerBrawler->SetHyperArmor(false);
        OwnerBrawler->SetBrawlerState(EBrawlerState::Dead);

        if (UBrawlerThrowableComponent* ThrowableComp = OwnerBrawler->FindComponentByClass<UBrawlerThrowableComponent>())
        {
            ThrowableComp->bCanBePickedUp = false;
        }

        const float MontageDuration = Montage ? OwnerBrawler->PlayAnimMontage(Montage) : 0.f;

        if (MontageDuration <= 0.f)
        {
            OwnerBrawler->FinishDeathAnimation();
        }

        OwnerBrawler->GetCharacterMovement()->DisableMovement();
        return;
    }

    // Damage is still applied, but hyper armor prevents interruption.
    if (OwnerBrawler->HasHyperArmor())
    {
        return;
    }

    // Do not restart hit reactions or apply repeated knockback while stunned.
    if (OwnerBrawler->GetBrawlerState() == EBrawlerState::Stunned)
    {
        return;
    }

    if (UBrawlerCombatComponent* CombatComp = OwnerBrawler->GetCombatComponent())
    {
        if (CombatComp->HasHitReactionImmunity())
        {
            return;
        }
    }

    if (Attacker)
    {
        FVector Direction = OwnerBrawler->GetActorLocation() - Attacker->GetActorLocation();
        Direction = Direction.GetSafeNormal();
        Direction.Z = 0.25f;
        Direction.Normalize();

        OwnerBrawler->LaunchCharacter(Direction * KnockbackAmount, true, true);
    }

    if (UBrawlerCombatComponent* CombatComp = OwnerBrawler->GetCombatComponent())
    {
        OwnerBrawler->SetBrawlerState(EBrawlerState::Stunned);
        CombatComp->PlayHitReaction(HitReactionType);
    }
}