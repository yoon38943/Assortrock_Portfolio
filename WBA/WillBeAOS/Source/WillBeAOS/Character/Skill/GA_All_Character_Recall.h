#pragma once

#include "CoreMinimal.h"
#include "GAS/WAbilitySystemComponent.h"
#include "GAS/WGameplayAbility.h"
#include "GA_All_Character_Recall.generated.h"

class URecallWidget;
class AWCharacterBase;

UCLASS()
class WILLBEAOS_API UGA_All_Character_Recall : public UWGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	float RecallTime = 8.f;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	static FGameplayTag GetRecallCueTag();

private:
	UPROPERTY()
	UAbilitySystemComponent* ASC;
	
	UPROPERTY()
	AWCharacterBase* Avatar;
	
	UPROPERTY()
	UAnimMontage* RecallMontage;

	UPROPERTY()
	UAnimMontage* CompleteRecallMontage;

	FTimerHandle RecallTimerHandle;
	
	void CallRecall_Server();
	void CallRecall_Client();
	void CompleteRecall_Server();
	void CompleteRecall_Client();
	void RecallToBase();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> RecallWidgetClass;

	UPROPERTY()
	URecallWidget* RecallWidget;

	void ShowRecallWidget();
	void HideRecallWidget();

	UFUNCTION()
	void OnRecallPressAgain(float TimeElapsed);
};
