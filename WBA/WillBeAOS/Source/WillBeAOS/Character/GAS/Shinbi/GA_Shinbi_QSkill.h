#pragma once

#include "CoreMinimal.h"
#include "GAS/WGameplayAbility.h"
#include "GA_Shinbi_QSkill.generated.h"

class AWCharacterBase;
class UAbilityTask_PlayMontageAndWait;
class AWolf;

UCLASS()
class WILLBEAOS_API UGA_Shinbi_QSkill : public UWGameplayAbility
{
	GENERATED_BODY()
	
public:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY()
	AWCharacterBase* Player;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitActors;

	UFUNCTION()
	void SpawnDashWolf(FGameplayEventData Data);

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TSubclassOf<AWolf> WolfClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* QSkill_Keydown_Montage;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* KeydownTask;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* QSkill_SpawnDashWolf_Montage;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void Shinbi_QSKill_DoDamage(FGameplayEventData Data);

	UPROPERTY(EditAnywhere, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> Shinbi_QSkill_DamageEffect;

	static FGameplayTag GetQSkillSpawnWolfEventTag();
	static FGameplayTag GetQSkillDashDamageEventTag();

	UPROPERTY()
	UDecalComponent* SpawnedDecal;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* SkillRangeDecalMaterial;
	
	void SpawnDashRangeDecal();
};
