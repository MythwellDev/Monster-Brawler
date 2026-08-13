#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerGrabComponent.generated.h"

class ABrawlerCharacter;
class UAnimMontage;
class UBrawlerThrowableComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KAIJUPROTOTYPE_API UBrawlerGrabComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBrawlerGrabComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* References */

	UPROPERTY()
	ABrawlerCharacter* OwnerBrawler = nullptr;

	/* Held Targets */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab")
	ABrawlerCharacter* GrabbedTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab")
	AActor* HeldThrowable = nullptr;

	UPROPERTY()
	ABrawlerCharacter* PendingGrabTarget = nullptr;

	UPROPERTY()
	AActor* PendingThrowable = nullptr;

	/* Settings */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	float GrabRange = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	float GrabRadius = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	float ThrowForce = 1800.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throw")
	float ThrowArcBias = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	FName GrabSocketName = "RightHand_GrabSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	FName GrabbedSocketName = "GrabbedSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab")
	FName ThrowableSocketName = "RightHand_ThrowableSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grab|Animation")
	UAnimMontage* GrabMontage = nullptr;

public:
	/* Main Input */

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void GrabOrThrow();

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void TryGrab();

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void ThrowGrabbedTarget();

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void ReleaseGrabbedTarget();

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void ConfirmGrab();

	UFUNCTION(BlueprintPure, Category = "Grab")
	bool IsGrabbing() const;

protected:
	/* Validation */

	bool CanGrab() const;

	/* Brawler Grab */

	ABrawlerCharacter* FindGrabbableTarget() const;
	void AttachTarget(ABrawlerCharacter* Target);
	ABrawlerCharacter* DetachGrabbedTarget();

	/* Throwable Grab */

	void TryGrabThrowable();
	AActor* FindThrowableTarget() const;
	void AttachThrowable(AActor* Throwable);

	UFUNCTION(BlueprintCallable, Category = "Grab")
	void ThrowHeldThrowable();

	/* Shared */

	FVector GetThrowDirection() const;

	/*=====================================
			Throwing
	=====================================*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throw")
	UAnimMontage* ThrowMontage = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Throw")
	void ConfirmThrow();

	UPROPERTY()
	AActor* CurrentThrowableCandidate = nullptr;

	void UpdateThrowableCandidate();
	void SetThrowableHighlight(AActor* Actor, bool bHighlighted);
};