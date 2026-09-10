#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerCombatTypes.h"
#include "BrawlerHealthComponent.generated.h"

class ABrawlerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBrawlerHealthChangedSignature, ABrawlerCharacter*, Brawler, float, PreviousHealth, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBrawlerDiedSignature, ABrawlerCharacter*, Victim, ABrawlerCharacter*, Killer);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KAIJUPROTOTYPE_API UBrawlerHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBrawlerHealthComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ABrawlerCharacter* OwnerBrawler;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	TObjectPtr<UAnimMontage> Montage = nullptr;

public:
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FBrawlerHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FBrawlerDiedSignature OnBrawlerDied;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ReceiveDamage(float DamageAmount, ABrawlerCharacter* Attacker, float KnockbackAmount, EHitReactionType HitReactionType);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;

	float GetCurrentHealth() const { return CurrentHealth; }
	float GetMaxHealth() const { return MaxHealth; }
};