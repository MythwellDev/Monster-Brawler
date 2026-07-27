#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerTargetingComponent.generated.h"

class ABrawlerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KAIJUPROTOTYPE_API UBrawlerTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBrawlerTargetingComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ToggleTargeting();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void StopTargeting();

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsTargeting() const { return CurrentTarget != nullptr; }

protected:
	UPROPERTY()
	ABrawlerCharacter* OwnerBrawler = nullptr;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetSearchRadius = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	float CameraPresetInterpSpeed = 6.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	float DefaultArmLength = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	float TargetingArmLength = 650.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	FVector DefaultSocketOffset = FVector(-250.f, 0.f, 175.f);

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	FVector TargetingSocketOffset = FVector(-150.f, 100.f, 225.f);

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	float TargetingYawOffset = -45.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting|Camera")
	float TargetingPitch = -10.f;

	void FindTarget();
	void UpdateCombatCamera(float DeltaTime);
};