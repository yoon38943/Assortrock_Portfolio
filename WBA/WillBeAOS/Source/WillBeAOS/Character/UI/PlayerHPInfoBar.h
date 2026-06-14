#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "PlayerHPInfoBar.generated.h"

UCLASS()
class WILLBEAOS_API UPlayerHPInfoBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

private:
	float BaseWidthSize = 300.f;
	float BaseHeightSize = 50.f;
	
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USizeBox* HealthBarSize;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PlayerNickName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UProgressBar> PlayerHPBar;

	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;

	void SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);
	void SetValue(float NewValue, float NewMaxValue);

	void ValueChanged(const FOnAttributeChangeData& Data);
	void MaxValueChanged(const FOnAttributeChangeData& Data);

	void SetBarScale(float Scale);

	float CachedValue;
	float CachedMaxValue;
};
