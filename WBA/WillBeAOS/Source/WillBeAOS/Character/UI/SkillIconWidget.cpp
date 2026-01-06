#include "Character/UI/SkillIconWidget.h"

#include "Character/Shinbi/Skill/SkillDataTable.h"
#include "GameFramework/GameStateBase.h"
#include "PersistentGame/GamePlayerState.h"


void USkillIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 쿨타임 텍스트 안 보이게
	if (CooldownText)
	{
		CooldownText->SetVisibility(ESlateVisibility::Hidden);
	}

	// 쿨타임 프로그레스바 안보이게
	if (CooldownProgress)
	{
		CooldownProgress->SetVisibility(ESlateVisibility::Hidden);
	}

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AGamePlayerState* PS = PC->GetPlayerState<AGamePlayerState>(); 
		if (PS)
		{
			PS->LoadSkillIcon.BindUObject(this, &ThisClass::LoadSkillIcon);
			PS->OnSkillCooldown.AddDynamic(this, &ThisClass::Handle_SkillUsed);
			if (SkillIcon && PS->InGamePlayerInfo.SelectedCharacter)
			{
				AWCharacterBase* DefaultCharacter = PS->InGamePlayerInfo.SelectedCharacter->GetDefaultObject<AWCharacterBase>();
				if (DefaultCharacter)
				{
					FName SkillIDName;
					switch (SkillID)
					{
					case ESkillSlot::Q:
						SkillIDName = "QSkill";
						break;
					case ESkillSlot::E:
						SkillIDName = "Eskill";
						break;
					case ESkillSlot::R:
						SkillIDName = "RSkill";
						break;
					}
					
					FSkillDataTable* DataTable = DefaultCharacter->SkillDataTable->FindRow<FSkillDataTable>(SkillIDName, TEXT(""));
					if (DataTable)
					{
						UTexture2D* SkillImage = DataTable->SkillIcon;
						SkillIcon->SetBrushFromTexture(SkillImage);
					}
				}
				
			}
		}
	}
}

void USkillIconWidget::LoadSkillIcon()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AGamePlayerState* PS = PC->GetPlayerState<AGamePlayerState>(); 
		if (PS)
		{
			if (SkillIcon)
			{
				AWCharacterBase* DefaultCharacter = PS->InGamePlayerInfo.SelectedCharacter->GetDefaultObject<AWCharacterBase>();
				if (DefaultCharacter)
				{
					FName SkillIDName;
					switch (SkillID)
					{
					case ESkillSlot::Q:
						SkillIDName = "QSkill";
						break;
					case ESkillSlot::E:
						SkillIDName = "Eskill";
						break;
					case ESkillSlot::R:
						SkillIDName = "RSkill";
						break;
					}
					
					FSkillDataTable* DataTable = DefaultCharacter->SkillDataTable->FindRow<FSkillDataTable>(SkillIDName, TEXT(""));
					if (DataTable)
					{
						UTexture2D* SkillImage = DataTable->SkillIcon;
						SkillIcon->SetBrushFromTexture(SkillImage);
					}
				}
				
			}
		}
	}
}

void USkillIconWidget::Handle_SkillUsed(FSkillUsedInfo SkillInfo)
{
	if (SkillID == SkillInfo.SkillSlot)
	{
		CooldownTime = SkillInfo.CooldownTime;
		CooldownTimeEnd = SkillInfo.CooldownTimeEnd;

		bIsCoolingDown = true;

		// 쿨타임 텍스트 보이게
		if (CooldownText)
		{
			CooldownText->SetVisibility(ESlateVisibility::Visible);
		}

		// 쿨타임 프로그레스바 보이게
		if (CooldownProgress)
		{
			CooldownProgress->SetVisibility(ESlateVisibility::Visible);
		}

		// 스킬 아이콘 흐리게
		if (SkillIcon)
		{
			SkillIcon->SetRenderOpacity(0.5f);
		}
	}
}

void USkillIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsCoolingDown)
	{
		return;
	}

	float CurrentTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	float RemaingTime = CooldownTimeEnd - CurrentTime;

	if (RemaingTime > 0.0f)
	{
		FString RemaingString = FString::Printf(TEXT("%.1f"), RemaingTime);
		FText RemaingText = FText::FromString(RemaingString);

		if (CooldownText)
		{
			CooldownText->SetText(RemaingText);
		}

		float DivideCoolTime = RemaingTime / CooldownTime;
		float RemainingPercent = FMath::Clamp(DivideCoolTime, 0, 1);

		if (CooldownProgress)
		{
			CooldownProgress->SetPercent(RemainingPercent);
		}
	}
	else
	{
		bIsCoolingDown = false;

		// 쿨타임 텍스트 안보이게
		if (CooldownText)
		{
			CooldownText->SetVisibility(ESlateVisibility::Hidden);
		}

		// 쿨타임 프로그레스바 안보이게
		if (CooldownProgress)
		{
			CooldownProgress->SetVisibility(ESlateVisibility::Hidden);
		}

		// 스킬 아이콘 뚜렷하게
		if (SkillIcon)
		{
			SkillIcon->SetRenderOpacity(1.f);
		}
	}
}
