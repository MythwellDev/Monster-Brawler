
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerCombatTypes.h"
#include "TimerManager.h"
#include "BrawlerThrowableComponent.generated.h"

class ABrawlerCharacter;
class UPrimitiveComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KAIJUPROTOTYPE_API UBrawlerThrowableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBrawlerThrowableComponent();

protected:
	virtual void BeginPlay() override;

public:

	/*=====================================
				Throwing
	=====================================*/

	// How fast this object is thrown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
	float ThrowSpeed = 1200.f;

	// Damage dealt when this object hits a brawler.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable|Impact")
	float ImpactDamage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable|Impact")
	float ImpactKnockback = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable|Impact")
	EHitReactionType ImpactReactionType;

	// Whether this object can currently be picked up.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
	bool bCanBePickedUp = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwable")
	FName AttachSocket = "RightHand_ThrowableSocket";

	//Position Adjustment applied after attaching to the carry socket.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwable")
	FVector AttachLocationOffset = FVector::ZeroVector;

	//Rotation Adjustment applied after attaching to the carry socket.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwable")
	FRotator AttachRotationOffset = FRotator::ZeroRotator;

	// Whether this object should face its travel direction when released.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwable|Flight")
	bool bOrientToThrowDirection = false;

	// Corrects for meshes whose point does not face the actor's local X axis.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwable|Flight")
	FRotator ThrowRotationOffset = FRotator::ZeroRotator;

	void BeginThrow(ABrawlerCharacter* InThrower);

protected:

	UFUNCTION()
	void HandleThrowableHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY()
	ABrawlerCharacter* Thrower = nullptr;

	UPROPERTY()
	UPrimitiveComponent* ThrowablePrimitive = nullptr;

	bool bIsInFlight = false;
	bool bHasDealtImpactDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwable|Flight")
	float MaximumFlightTime = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwable|Flight")
	float MinimumDamagingSpeed = 200.f;

	FTimerHandle FlightTimerHandle;

	void FinishThrow();
};