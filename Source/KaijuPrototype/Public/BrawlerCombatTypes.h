#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "Templates/SubclassOf.h"
#include "BrawlerCombatTypes.generated.h"

class UCameraShakeBase;
class USoundBase;

UENUM(BlueprintType)
enum class EHitReactionType : uint8
{
	Light UMETA(DisplayName = "Light"),
	Heavy UMETA(DisplayName = "Heavy"),
	Knockdown UMETA(DisplayName = "Knockdown"),
	Launch UMETA(DisplayName = "Launch"),
	Stagger UMETA(DisplayName = "Stagger")
};

USTRUCT(BlueprintType)
struct FBrawlerHitReactData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit React")
	EHitReactionType HitReactionType = EHitReactionType::Light;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> Montage = nullptr;
};

USTRUCT(BlueprintType)
struct FBrawlerImpactFeedbackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	float HitStopDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	TObjectPtr<USoundBase> ImpactSound = nullptr;
};

USTRUCT(BlueprintType)
struct FBrawlerAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FName AttackName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FName TraceStartSocket = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FName TraceEndSocket = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float TraceRadius = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float Range = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float Knockback = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit React")
	EHitReactionType HitReactionType = EHitReactionType::Light;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Feedback")
	FBrawlerImpactFeedbackData ImpactFeedback;
};