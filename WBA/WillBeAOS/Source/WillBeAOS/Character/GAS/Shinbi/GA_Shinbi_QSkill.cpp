#include "Character/GAS/Shinbi/GA_Shinbi_QSkill.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/Shinbi/Wolf/Wolf.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"


void UGA_Shinbi_QSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CanActivateAbility(Handle, ActorInfo))
	{
		K2_EndAbility();
		return;
	}

	HitActors.Empty();

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;
	
	Player = Cast<AWCharacterBase>(Avatar);

	if (ActorInfo->IsLocallyControlled())
	{
		SpawnDashRangeDecal();
	}

	if (K2_HasAuthority())
	{		
		UAbilityTask_WaitGameplayEvent* WaitDashEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetQSkillDashDamageEventTag());
		WaitDashEventTask->EventReceived.AddDynamic(this, &ThisClass::Shinbi_QSKill_DoDamage);
		WaitDashEventTask->ReadyForActivation();
	}

	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	if (Spec && !Spec->InputPressed)
	{
		OnInputReleased(0.f);
	}
	else
	{
		KeydownTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, QSkill_Keydown_Montage);
		KeydownTask->ReadyForActivation();
		
		UAbilityTask_WaitInputRelease* WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
		WaitReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		WaitReleaseTask->ReadyForActivation();
	}
}

void UGA_Shinbi_QSkill::SpawnDashWolf(FGameplayEventData Data)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;
	
	FVector OwnerCharacterLocation = AvatarActor->GetActorLocation();
	FRotator OwnerCharacterRotation = AvatarActor->GetActorRotation();
	FVector ForwardVector = AvatarActor->GetActorForwardVector();

	float SpawDistance = 100.f;
	FVector SpawnLocation = OwnerCharacterLocation + (ForwardVector * SpawDistance);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(AvatarActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWolf* Wolf = GetWorld()->SpawnActor<AWolf>(
		WolfClass,
		SpawnLocation,
		OwnerCharacterRotation,
		SpawnParams
	);

	if (Wolf)
	{
		Wolf->LaunchWolf(AvatarActor);
	}
}

void UGA_Shinbi_QSkill::OnInputReleased(float TimeHeld)
{
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

	if (!CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), ActivationInfo))
	{
		K2_EndAbility();
		return;
	}
	
	if (Player->SkillForwardDecal)
	{
		Player->SkillForwardDecal->DestroyComponent();
		Player->SkillForwardDecal = nullptr;
	}
	
	if (KeydownTask)
	{
		KeydownTask->EndTask();
		KeydownTask = nullptr;
	}
	
	if (HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &ActivationInfo))
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
		
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, QSkill_SpawnDashWolf_Montage);
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
		PlayComboMontageTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitSpawnWolfEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetQSkillSpawnWolfEventTag());
		WaitSpawnWolfEventTask->EventReceived.AddDynamic(this, &ThisClass::SpawnDashWolf);
		WaitSpawnWolfEventTask->ReadyForActivation();
	}
}

void UGA_Shinbi_QSkill::Shinbi_QSKill_DoDamage(FGameplayEventData Data)
{
	AActor* HitActor = const_cast<AActor*>(Data.Target.Get());
	if (!HitActor) return;

	if (HitActors.Contains(HitActor)) return;
	HitActors.Add(HitActor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC) return;

	FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	
	if (Data.TargetData.IsValid(0))
	{
		if (Data.TargetData.Get(0)->GetHitResult())
		{
			const FHitResult* HitResult = Data.TargetData.Get(0)->GetHitResult();
			if (HitResult)
			{
				EffectContext.AddHitResult(*HitResult);
			}
		}
		
		if (!Data.TargetData.Get(0)->GetActors().IsEmpty())
		{
			TArray<TWeakObjectPtr<AActor>> ActorArray;
			for (auto& Actor : Data.TargetData.Get(0)->GetActors())
			{
				ActorArray.Add(Actor);
			}
			
			EffectContext.AddActors(ActorArray);
		}
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Shinbi_QSkill_DamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetContext(EffectContext);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		FGameplayTag::RequestGameplayTag("ability.shinbi.qskill.damageratio"),
		1.5f
	);

	FGameplayAbilityTargetDataHandle TargetDataHandle = Data.TargetData;
	
	(void)ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, SpecHandle, TargetDataHandle);
}

FGameplayTag UGA_Shinbi_QSkill::GetQSkillSpawnWolfEventTag()
{
	return FGameplayTag::RequestGameplayTag("ability.shinbi.qskill.spawn");
}

FGameplayTag UGA_Shinbi_QSkill::GetQSkillDashDamageEventTag()
{
	return FGameplayTag::RequestGameplayTag("ability.shinbi.qskill.damage");
}

void UGA_Shinbi_QSkill::SpawnDashRangeDecal()
{	
	if (!Player) return;

	FVector SpawnLocation = Player->GetActorLocation() + Player->GetActorForwardVector() * 700.f;
	FRotator SpawnRotation = Player->GetActorRotation();

	SpawnRotation.Pitch -= 90.f;
	SpawnRotation.Yaw -= 90.f;

	SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(),
		SkillRangeDecalMaterial,
		FVector(100.f, 600.f, 50.f),
		SpawnLocation,
		SpawnRotation
	);

	if (SpawnedDecal)
	{
		Player->SkillForwardDecal = SpawnedDecal;
	}
}
