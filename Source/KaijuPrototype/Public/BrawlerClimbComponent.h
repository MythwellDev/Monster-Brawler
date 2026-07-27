

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerClimbComponent.generated.h"

class ABrawlerCharacter;
class UAnimMontage;
class UCameraShakeBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KAIJUPROTOTYPE_API UBrawlerClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBrawlerClimbComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ABrawlerCharacter* OwnerBrawler;

	UPROPERTY()
	AActor* CurrentClimbActor = nullptr;

	UPROPERTY()
	FVector CurrentWallNormal = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climbing")
	float WallOffset = 125.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climbing")
	bool bIsClimbing = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climbing")
	float ClimbTraceDistance = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climbing")
	float ClimbSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climbing")
	FName ClimbContactSocketName = "ClimbContactSocket";

	UPROPERTY()
	float CurrentClimbInput;

	bool IsNearGround() const;
	bool IsWallStillClimbable() const;
	bool TryMantleToTop();
	void StartMantle(const FVector& TargetLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climbing|Mantle")
	UAnimMontage* MantleMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Climbing|Feedback")
	TSubclassOf<UCameraShakeBase> MantleCameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Climbing|Mantle")
	FVector MantleTargetLocation = FVector::ZeroVector;

	UPROPERTY()
	float ClimbGroundZ = 0.f;

	bool IsAtClimbGround() const;

public:	
	UFUNCTION(BlueprintCallable)
	void ResetClimbInput();

	UFUNCTION(BlueprintCallable)
	void TryStartClimb();

	UFUNCTION(BlueprintCallable)
	void StopClimb();

	UFUNCTION(BlueprintCallable)
	void ClimbMove(float Value);

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const { return bIsClimbing; }

	UFUNCTION(BlueprintPure)
	float GetCurrentClimbInput() const { return CurrentClimbInput; }

	UFUNCTION(BlueprintCallable, Category = "Climbing|Mantle")
	void FinishMantle();

	void PlayMantleCameraShake();
};
