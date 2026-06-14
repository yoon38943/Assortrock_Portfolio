#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UWGameplayAbilityTypes.h"
#include "WAbilitySystemComponent.generated.h"

UCLASS()
class WILLBEAOS_API UWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void ApplyInitialEffects();

	void GiveInitialAbilities();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TMap<EWAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TMap<EWAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;
};
