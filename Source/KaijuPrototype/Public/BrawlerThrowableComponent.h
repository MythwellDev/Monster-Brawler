
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerThrowableComponent.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
	float ImpactDamage = 25.f;

	// Whether this object can currently be picked up.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwable")
	bool bCanBePickedUp = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwable")
	FName AttachSocket = "RightHand_ThrowableSocket";
};