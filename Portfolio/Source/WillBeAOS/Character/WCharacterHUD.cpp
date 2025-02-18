#include "WCharacterHUD.h"
#include "WCharacterBase.h"
#include "WPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "../Game/WGameState.h"


void UWCharacterHUD::NativeConstruct()
{
	Super::NativeConstruct();

	AWGS = GetWorld()->GetGameState<AWGameState>();

	AWC = Cast<AWCharacterBase>(GetOwningPlayerPawn());
	if (AWC)
	{
		AWC->DSkillLCooldown.BindUObject(this, &ThisClass::SetSkillLTimer);
	}
	
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		AWPS = PlayerController->GetPlayerState<AWPlayerState>();

		if (AWPS)
		{
			auto Message = FString::Printf(TEXT("PlayerState 가져오기 성공: %s"), *AWPS->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
		}
		else
		{
			auto Message = FString::Printf(TEXT("PlayerState가 NULL. 0.5초 후 재시도."));
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
			GetWorld()->GetTimerManager().SetTimer(ErrorTimerHandle, this, &UWCharacterHUD::TryGetPlayerState, 0.5f, false);
		}
	}

	UpdateCharacter();
}

void UWCharacterHUD::TryGetPlayerState()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		AWPS = PlayerController->GetPlayerState<AWPlayerState>();

		if (AWPS)
		{
			auto Message = FString::Printf(TEXT("PlayerState 가져오기 성공: %s"), *AWPS->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
			UpdateCharacter();  // ✅ UI 업데이트
			return;
		}
	}
	
	auto Message = FString::Printf(TEXT("여전히 PlayerState가 NULL. 0.5초 후 재시도."));
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
	GetWorld()->GetTimerManager().SetTimer(ErrorTimerHandle, this, &UWCharacterHUD::TryGetPlayerState, 0.5f, false);
}

void UWCharacterHUD::UpdateCharacter()
{
	AWC = Cast<AWCharacterBase>(GetOwningPlayerPawn());
	
	if (AWC)
	{
		AWC->DSkillLCooldown.BindUObject(this, &ThisClass::SetSkillLTimer);
		AWC->DSkillRCooldown.BindUObject(this, &ThisClass::SetSkillRTimer);
	}
	// Initialize Skill L Data
	SkillLData.SkillCooldown = SkillLCooldown;
	SkillLData.SkillProgress = Skill_LProgress;
	SkillLData.SkillTimer = Skill_LTimer;

	// Initialize Skill R Data
	SkillRData.SkillCooldown = SkillRCooldown;
	SkillRData.SkillProgress = Skill_RProgress;
	SkillRData.SkillTimer = Skill_RTimer;

	// init 스탯
	SetState();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SetState, 0.5f, true);
}

float UWCharacterHUD::GetHealthBarPercentage()
{
	if (AWPS == nullptr)
	{
		return 0.0f;
	}
		
	return AWPS->GetHP() / AWPS->GetMaxHP();
}

void UWCharacterHUD::SetSkillTimer(FSkillCooldownData& SkillData)
{
	SkillData.CurrentSkillCooldown = SkillData.SkillCooldown;
}

FText UWCharacterHUD::ShowSkillTimer(FSkillCooldownData& SkillData)
{
	FString CooldownString;
	if(SkillData.CurrentSkillCooldown<1)
	{
		CooldownString = FString::Printf(TEXT("%.1f"), SkillData.CurrentSkillCooldown);
	}
	else CooldownString = FString::Printf(TEXT("%d"), int32 (SkillData.CurrentSkillCooldown));
	return FText::FromString(CooldownString);
}

float UWCharacterHUD::ShowSkillProgress(FSkillCooldownData& SkillData)
{
	return SkillData.CurrentSkillCooldown / SkillData.SkillCooldown;
}


void UWCharacterHUD::SetSkillLTimer()
{
	SetSkillTimer(SkillLData);
	GetWorld()->GetTimerManager().SetTimer(CooldownLTimerHandle, this, &ThisClass::UpdateSkillLTimer, 0.1f, true);
}

FText UWCharacterHUD::ShowSkillLTimer()
{
	return ShowSkillTimer(SkillLData);
}

float UWCharacterHUD::ShowSkillLProgress()
{
	return ShowSkillProgress(SkillLData);
}

void UWCharacterHUD::UpdateSkillLTimer()
{
	if (SkillLData.CurrentSkillCooldown <= 0.1f)
	//0으로 지정할시 -0.1에 clear됨...
	//왜지...
	//0.1f에 지정했더니 0.0에 제대로 들어감
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownLTimerHandle);
		SkillLData.SkillProgress->SetVisibility(ESlateVisibility::Hidden);
		SkillLData.SkillTimer->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SkillLData.CurrentSkillCooldown -= 0.1f;
		SkillLData.SkillProgress->SetVisibility(ESlateVisibility::Visible);
		SkillLData.SkillTimer->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWCharacterHUD::SetSkillRTimer()
{
	SetSkillTimer(SkillRData);
	GetWorld()->GetTimerManager().SetTimer(CooldownRTimerHandle, this, &ThisClass::UpdateSkillRTimer, 0.1f, true);
}

FText UWCharacterHUD::ShowSkillRTimer()
{
	return ShowSkillTimer(SkillRData);
}

float UWCharacterHUD::ShowSkillRProgress()
{
	return ShowSkillProgress(SkillRData);
}

void UWCharacterHUD::UpdateSkillRTimer()
{
	if (SkillRData.CurrentSkillCooldown <= 0.1f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownRTimerHandle);
		SkillRData.SkillProgress->SetVisibility(ESlateVisibility::Hidden);
		SkillRData.SkillTimer->SetVisibility(ESlateVisibility::Hidden);
		if (AWC)
		{
			AWC->SkillREnable = false;
		}
	}
	else
	{
		SkillRData.CurrentSkillCooldown -= 0.1f;
		SkillRData.SkillProgress->SetVisibility(ESlateVisibility::Visible);
		SkillRData.SkillTimer->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWCharacterHUD::SetState()
{
	if (AWPS)
	{
		FString PowerString = FString::Printf(TEXT("Attack: %d"), AWPS->CPower);
		Power->SetText(FText::FromString(PowerString));

		FString AHString = FString::Printf(TEXT("AddHealth: %d"), AWPS->CAdditionalHealth);
		AdditionalHealth->SetText(FText::FromString(AHString));

		FString DefenceString = FString::Printf(TEXT("Def: %d"), AWPS->CDefense);
		Defence->SetText(FText::FromString(DefenceString));

		FString SpeedString = FString::Printf(TEXT("Speed: %.1f"), AWPS->CSpeed);
		Speed->SetText(FText::FromString(SpeedString));

		FString CExpString = FString::Printf(TEXT("%d / "), AWPS->CCurrentExp);
		Exp->SetText(FText::FromString(CExpString));

		FString MExpString = FString::Printf(TEXT("%d"), AWPS->CExperience);
		Max_Exp->SetText(FText::FromString(MExpString));

		FString LevelString = FString::Printf(TEXT("Level: %d"), AWPS->CLevel);
		Level->SetText(FText::FromString(LevelString));

		FString AbLevelString = FString::Printf(TEXT("ALv: %d"), AWPS->CAbLevel);
		AbilityLevel->SetText(FText::FromString(AbLevelString));

		FString HealthString = FString::Printf(TEXT("%.f / %.f"), AWPS->GetHP(), AWPS->GetMaxHP());
		CurrentHP->SetText(FText::FromString(HealthString));
		
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
}

void UWCharacterHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AWPS)
	{
		SetState();
	}
}


FText UWCharacterHUD::SetGold()
{
	if (AWPS)
	{
		FString GoldString = FString::Printf(TEXT("Gold: %d"), AWPS->CGold);
		return FText::FromString(GoldString);
	}
	return FText();
}