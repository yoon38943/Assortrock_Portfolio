#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/Skill/SkillType.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "SkillIconWidget.generated.h"

UCLASS()
class WILLBEAOS_API USkillIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void Handle_SkillUsed(FSkillUsedInfo SkillInfo);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CooldownText;

	UPROPERTY(EditAnywhere, Category = "Skill")
	ESkillSlot SkillID;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* CooldownProgress;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillIcon;

	void LoadSkillIcon();

private:
	bool bIsCoolingDown = false;

	float CooldownTime = 0.0f;
	
	float CooldownTimeEnd = 0.0f;
};
