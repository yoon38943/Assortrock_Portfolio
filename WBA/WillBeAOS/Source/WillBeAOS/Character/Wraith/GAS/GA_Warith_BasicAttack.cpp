#include "Character/Wraith/GAS/GA_Warith_BasicAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"


void UGA_Warith_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* SendEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetBasicAttackEventTag());
		SendEventTask->EventReceived.AddDynamic(this, &ThisClass::JumpToNextShoot);
	}
}

void UGA_Warith_BasicAttack::PerformAttack()
{
	if (HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &GetCurrentActivationInfoRef()))
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, BasicAttack_Montage);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->ReadyForActivation();
	}

	ShootAttack();
}

void UGA_Warith_BasicAttack::ShootAttack()
{
	
}

FGameplayTag UGA_Warith_BasicAttack::GetBasicAttackEventTag()
{
	return FGameplayTag::RequestGameplayTag("ability.basicattack");
}

void UGA_Warith_BasicAttack::JumpToNextShoot(FGameplayEventData Data)
{
	
}
