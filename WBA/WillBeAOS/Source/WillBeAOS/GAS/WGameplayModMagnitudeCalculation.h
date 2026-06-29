#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "WGameplayModMagnitudeCalculation.generated.h"

UCLASS()
class WILLBEAOS_API UWGameplayModMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UWGameplayModMagnitudeCalculation();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
};
