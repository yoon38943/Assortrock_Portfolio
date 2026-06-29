#include "GAS/WGameplayAbility.h"

#include "Character/WCharacterBase.h"

class UAnimInstance* UWGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

void UWGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		AActor* Avatar = GetAvatarActorFromActorInfo();
		if (Avatar)
		{
			AWCharacterBase* PlayerAvatar = Cast<AWCharacterBase>(Avatar);
			if (PlayerAvatar)
			{
				FRotator CameraRot = PlayerAvatar->GetController()->GetControlRotation();
				PlayerAvatar->SetActorRotation(FRotator(0, CameraRot.Yaw, 0));
			}
		}
	}
}
