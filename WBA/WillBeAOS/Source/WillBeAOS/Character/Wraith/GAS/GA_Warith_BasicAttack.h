#pragma once

#include "CoreMinimal.h"
#include "GAS/WGameplayAbility.h"
#include "GA_Warith_BasicAttack.generated.h"

UCLASS()
class WILLBEAOS_API UGA_Warith_BasicAttack : public UWGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* BasicAttack_Montage;

	void PerformAttack();

	void ShootAttack();

	static FGameplayTag GetBasicAttackEventTag();

	UFUNCTION()
	void JumpToNextShoot(FGameplayEventData Data);
};
