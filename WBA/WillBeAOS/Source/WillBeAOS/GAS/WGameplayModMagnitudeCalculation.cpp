#include "GAS/WGameplayModMagnitudeCalculation.h"

#include "WAttributeSet.h"


UWGameplayModMagnitudeCalculation::UWGameplayModMagnitudeCalculation()
{
	AttackPowerDef.AttributeToCapture = UWAttributeSet::GetAttackStatAttribute();
	AttackPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AttackPowerDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AttackPowerDef);
}

float UWGameplayModMagnitudeCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	float AttackPower = 0.f;
	GetCapturedAttributeMagnitude(AttackPowerDef, Spec, EvalParams, AttackPower);

	float SkillRatio = Spec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("ability.data.damage"),
		false,
		1.0f
	);

	float FinalDamage = AttackPower * SkillRatio;

	UE_LOG(LogTemp, Display, TEXT("Damage : %f"), FinalDamage);

	return -FinalDamage;
}
