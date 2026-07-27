#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrawlerCombatTypes.h"
#include "BrawlerCombatComponent.generated.h"

class ABrawlerCharacter;
class UAnimMontage;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KAIJUPROTOTYPE_API UBrawlerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBrawlerCombatComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	ABrawlerCharacter* OwnerBrawler;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks")
	TArray<FBrawlerAttackData> LightAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attacks")
	TArray<FBrawlerAttackData> HeavyAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit Reactions")
	TArray<FBrawlerHitReactData> HitReactions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attacks")
	FBrawlerAttackData CurrentAttack;

	const TArray<FBrawlerAttackData>* CurrentAttackSet = nullptr;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	/* Combo System */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combos")
	int32 CurrentComboIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combos")
	bool bCanQueueNextAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combos")
	bool bHasQueuedAttack = false;

	/* Blocking System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blocking")
	TObjectPtr<UAnimMontage> BlockMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blocking")
	float BlockDamageMultiplier = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	bool bIsBlocking = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void LightAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OpenComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CloseComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformHitTrace();

	void PlayAttackCameraShake();
	void PlayHitStop(float Duration);

	void PlayHitReaction(EHitReactionType ReactionType);

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	void StartBlock();

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	void StopBlock();

	UFUNCTION(BlueprintPure, Category = "Blocking")
	bool IsBlocking() const { return bIsBlocking; }

	UFUNCTION(BlueprintPure, Category = "Blocking")
	float GetBlockDamageMultiplier() const { return BlockDamageMultiplier; }

protected:
	void StartAttackFromSet(const TArray<FBrawlerAttackData>& AttackSet, int32 AttackIndex);
	void PlayAttackMontage(const FBrawlerAttackData& AttackData);
	void ResetCombo();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnBlockMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};