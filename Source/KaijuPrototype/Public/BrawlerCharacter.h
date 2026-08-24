#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BrawlerCombatTypes.h"
#include "BrawlerCharacter.generated.h"

class UBrawlerCombatComponent;
class UBrawlerHealthComponent;
class UBrawlerGrabComponent;
class UBrawlerClimbComponent;
class UBrawlerTargetingComponent;
class UBrawlerThrowableComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EBrawlerState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Attacking UMETA(DisplayName = "Attacking"),
	Stunned UMETA(DisplayName = "Stunned"),
	Grabbing UMETA(DisplayName = "Grabbing"),
	Grabbed UMETA(DisplayName = "Grabbed"),
	Climbing UMETA(DisplayName = "Climbing"),
	Mantling UMETA(DisplayName = "Mantling"),
	Blocking UMETA(DIsplayName = "Blocking"),
	Throwing UMETA(DisplayName = "Throwing"),
	Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class KAIJUPROTOTYPE_API ABrawlerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABrawlerCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|Components")
	UBrawlerCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|Components")
	UBrawlerHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|Components")
	UBrawlerGrabComponent* GrabComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|Components")
	UBrawlerClimbComponent* ClimbComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|Components")
	UBrawlerTargetingComponent* TargetingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|Components")
	UBrawlerThrowableComponent* ThrowableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brawler|State")
	EBrawlerState CurrentState = EBrawlerState::Idle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brawler|Movement")
	float WalkSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Brawler|Movement")
	float SprintSpeed = 500.f;

	virtual void Landed(const FHitResult& Hit) override;

	bool bRecoverFromThrowOnLanding = false;

public:

	/*======================================
			State Accessors
	======================================*/

	UFUNCTION(BlueprintCallable, Category = "Brawler|State")
	void SetBrawlerState(EBrawlerState NewState);

	UFUNCTION(BlueprintPure, Category = "Brawler|State")
	EBrawlerState GetBrawlerState() const;


	/*======================================
			Health Component Accessors
	=======================================*/

	UFUNCTION(BlueprintCallable, Category = "Brawler|Health")
	void ReceiveDamage(float DamageAmount, ABrawlerCharacter* Attacker, float KnockbackAmount, EHitReactionType HitReactionType);

	UBrawlerHealthComponent* GetHealthComponent() const { return HealthComponent; }


	/*=======================================
			Combat Component Accessors
	=======================================*/

	UFUNCTION(BlueprintPure, Category = "Brawler|State")
	bool CanAttack() const;

	UFUNCTION(BlueprintPure, Category = "Brawler|State")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Brawler|Attacks")
	void LightAttack();

	UFUNCTION(BlueprintCallable, Category = "Brawler|Attacks")
	void HeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Brawler|Attacks")
	void PerformHitTrace();

	UBrawlerCombatComponent* GetCombatComponent() const { return CombatComponent; }


	/*=======================================
			Grab Component Accessors
	========================================*/

	UFUNCTION(BlueprintCallable, Category = "Brawler|Grab")
	void TryGrab();

	UFUNCTION(BlueprintCallable, Category = "Brawler|Grab")
	void ThrowGrabbedTarget();

	UFUNCTION(BlueprintPure, Category = "Brawler|Grab")
	bool IsGrabbing() const;

	UBrawlerGrabComponent* GetGrabComponent() const { return GrabComponent; }
	
	void BeginThrownState();
	
	/*=======================================
			Climb Component Accessors
	=======================================*/

	UBrawlerClimbComponent* GetClimbComponent() const { return ClimbComponent; }


	/*=======================================
		Targeting Component Accessors
	=======================================*/

	UBrawlerTargetingComponent* GetTargetingComponent() const { return TargetingComponent; }

	/*=======================================
				Death
=======================================*/

	UFUNCTION(BlueprintCallable, Category = "Brawler|Death")
	void FinishDeathAnimation();
};