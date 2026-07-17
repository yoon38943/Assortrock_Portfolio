#include "Character/UI/SkillIconWidget.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Character/Skill/SkillDataTable.h"
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

	AWCharacterBase* OwnerChar = Cast<AWCharacterBase>(GetOwningPlayerPawn());
	if (OwnerChar)
	{
		OwnerASC = OwnerChar->GetAbilitySystemComponent();

		FName SkillIDName;
		switch (SkillID)
		{
		case ESkillSlot::RM:
			SkillIDName = "RMSkill";
			break;
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
			
		FSkillDataTable* DataTable = OwnerChar->SkillDataTable->FindRow<FSkillDataTable>(SkillIDName, TEXT(""));
		if (DataTable)
		{
			UTexture2D* SkillImage = DataTable->SkillIcon;
			SkillIcon->SetBrushFromTexture(SkillImage);
		}

		OwnerASC->RegisterGameplayTagEvent(
			CooldownTag,
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::Handle_SkillUsed);

		if (InternalCooldownTag.IsValid())
		{
			OwnerASC->RegisterGameplayTagEvent(
				InternalCooldownTag,
				EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::Handle_InternalSkillUsed);
		}
	}
	
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetOwningPlayer());
	if (PS)
	{
		PS->LoadSkillIcon.BindUObject(this, &ThisClass::LoadSkillIcon);
		//PS->OnSkillCooldown.AddDynamic(this, &ThisClass::Handle_SkillUsed);
		
		/*if (SkillIcon && PS->InGamePlayerInfo.SelectedCharacter)
		{
			AWCharacterBase* DefaultCharacter = PS->InGamePlayerInfo.SelectedCharacter->GetDefaultObject<AWCharacterBase>();
		}*/
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
						SkillIDName = "ESkill";
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

void USkillIconWidget::CooldownUpdate(bool Isinternal)
{
	if (!OwnerASC) return;

	float TimeRemaining = 0.f;
	float Duration = 0.f;

	FGameplayEffectQuery Query;
	if (!Isinternal)
		Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag));
	else
		Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(InternalCooldownTag));

	TArray<TPair<float, float>> Results = OwnerASC->GetActiveEffectsTimeRemainingAndDuration(Query);

	if (Results.Num() > 0)
	{
		for (const auto& Result: Results)
		{
			if (Result.Key > TimeRemaining)
			{
				TimeRemaining = Result.Key;
				Duration = Result.Value;
			}
		}
	}
	
	if (TimeRemaining > 0.f)
	{
		float Percent = TimeRemaining / Duration;
		float RemainingPercent = FMath::Clamp(Percent, 0, 1);
		
		if (CooldownProgress)
			CooldownProgress->SetPercent(RemainingPercent);

		FString RemaingString = FString::Printf(TEXT("%.1f"), TimeRemaining);
		FText RemaingText = FText::FromString(RemaingString);
		if (CooldownText)
			CooldownText->SetText(RemaingText);
	}
	else
	{
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

		bIsCoolingDown = false;
	}
}

void USkillIconWidget::Handle_SkillUsed(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		bIsInternal = false;
		bIsCoolingDown = true;

		// 쿨타임 텍스트 보이게
		if (CooldownText)
		{
			CooldownText->SetVisibility(ESlateVisibility::Visible);
		}

		// 쿨타임 프로그레스바 보이게
		if (CooldownProgress)
		{
			CooldownProgress->SetFillColorAndOpacity(CooldownColor);
			CooldownProgress->SetVisibility(ESlateVisibility::Visible);
		}

		// 스킬 아이콘 흐리게
		if (SkillIcon)
		{
			SkillIcon->SetRenderOpacity(0.5f);
		}
	}
}

void USkillIconWidget::Handle_InternalSkillUsed(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("내부쿨 사용중"));
		bIsInternal = true;
		bIsCoolingDown = true;

		// 쿨타임 텍스트 보이게
		if (CooldownText)
		{
			CooldownText->SetVisibility(ESlateVisibility::Visible);
		}

		// 쿨타임 프로그레스바 보이게
		if (CooldownProgress)
		{
			CooldownProgress->SetFillColorAndOpacity(InternalCooldownColor);
			CooldownProgress->SetVisibility(ESlateVisibility::Visible);
		}

		/*// 스킬 아이콘 흐리게
		if (SkillIcon)
		{
			SkillIcon->SetRenderOpacity(0.5f);
		}*/
	}
}

void USkillIconWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!GetOwningPlayerPawn()->IsLocallyControlled()) return;
	if (!bIsCoolingDown) return;

	if (bIsInternal)
		CooldownUpdate(true);
	else
		CooldownUpdate(false);
}
