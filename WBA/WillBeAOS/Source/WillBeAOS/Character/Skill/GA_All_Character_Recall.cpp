#include "Character/Skill/GA_All_Character_Recall.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Blueprint/UserWidget.h"
#include "Character/WCharacterBase.h"
#include "Gimmick/PlayerSpawner.h"
#include "PersistentGame/GamePlayerController.h"
#include "PersistentGame/GamePlayerState.h"
#include "Widget/RecallWidget.h"


void UGA_All_Character_Recall::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASC = GetAbilitySystemComponentFromActorInfo();

	Avatar = Cast<AWCharacterBase>(GetAvatarActorFromActorInfo());
	if (Avatar)
	{
		RecallMontage = Avatar->GetStartRecallMontage();
		CompleteRecallMontage = Avatar->GetCompleteRecallMontage();
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, RecallMontage);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
		MontageTask->ReadyForActivation();
	}

	if (K2_HasAuthority())
	{
		CallRecall_Server();
	}
	else
	{
		CallRecall_Client();
	}

	if (ASC && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
		ASC->AddGameplayCue(GetRecallCueTag());

	UAbilityTask_WaitInputPress* WaitInputTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputTask->OnPress.AddDynamic(this, &ThisClass::OnRecallPressAgain);
	WaitInputTask->ReadyForActivation();
}

void UGA_All_Character_Recall::CallRecall_Server()
{
	Avatar->IsRecalling = true;

	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &ThisClass::CompleteRecall_Server, RecallTime, false);
}

void UGA_All_Character_Recall::CallRecall_Client()
{
	if (IsLocallyControlled())
	{
		ShowRecallWidget();
	}

	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &ThisClass::CompleteRecall_Client, RecallTime, false);
}

void UGA_All_Character_Recall::CompleteRecall_Server()
{
	Avatar->IsRecalling = false;
	
	RecallToBase();

	if (Avatar && CompleteRecallMontage)
	{
		Avatar->MultiPlayMontage(CompleteRecallMontage);
	}

	K2_EndAbility();
}

void UGA_All_Character_Recall::CompleteRecall_Client()
{
	if (IsLocallyControlled())
	{
		HideRecallWidget();
	}
}

void UGA_All_Character_Recall::RecallToBase()
{
	AGamePlayerController* Controller = Cast<AGamePlayerController>(Avatar->GetController());
	if (!Controller) return;

	AGamePlayerState* WPlayerState = Cast<AGamePlayerState>(Controller->PlayerState);
	if (!WPlayerState) return;
	
	if (AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		PlayerChar->SetActorLocation(WPlayerState->PlayerSpawner->GetActorLocation());
		FRotator LookCenter = (FVector(0, 0, 100) - PlayerChar->GetActorLocation()).Rotation();
		PlayerChar->SetActorRotation(LookCenter);
		PlayerChar->GetController()->SetControlRotation(LookCenter);
	}
}

void UGA_All_Character_Recall::ShowRecallWidget()
{
	if (RecallWidgetClass && !RecallWidget)
	{
		RecallWidget = CreateWidget<URecallWidget>(Cast<AGamePlayerController>(Avatar->GetController()), RecallWidgetClass);
		if (RecallWidget)
		{
			RecallWidget->RecallTime = RecallTime;
			RecallWidget->StartRecalling();
			RecallWidget->AddToViewport();
		}
	}
}

void UGA_All_Character_Recall::HideRecallWidget()
{
	if (RecallWidget && RecallWidget->IsInViewport())
	{
		RecallWidget->RemoveFromParent();
		RecallWidget = nullptr;
	}
}

void UGA_All_Character_Recall::OnRecallPressAgain(float TimeElapsed)
{
	K2_CancelAbility();
}

void UGA_All_Character_Recall::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateEndAbility, bool bWasCancelled)
{
	HideRecallWidget();

	Avatar->IsRecalling = false;

	if (ASC && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
		ASC->RemoveGameplayCue(GetRecallCueTag());
	
	if (RecallTimerHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_All_Character_Recall::GetRecallCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.state.recall");
}
