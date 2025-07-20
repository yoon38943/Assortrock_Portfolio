#include "WCharacterHUD.h"

#include "WCharacterBase.h"
#include "Components/TextBlock.h"
#include "PersistentGame/GamePlayerState.h"


void UWCharacterHUD::NativeConstruct()
{
	Super::NativeConstruct();

	AWGS = GetWorld()->GetGameState<APlayGameState>();
	
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		AWPS = PlayerController->GetPlayerState<AGamePlayerState>();

		if (AWPS)
		{
			auto Message = FString::Printf(TEXT("PlayerState 가져오기 성공: %s"), *AWPS->GetName());
		}
		else
		{
			// PlayerState가 NULL. 0.5초 후 재시도
			GetWorld()->GetTimerManager().SetTimer(ErrorTimerHandle, this, &UWCharacterHUD::TryGetPlayerState, 0.2f, true);
		}
	}

	UpdateCharacter();
}

void UWCharacterHUD::TryGetPlayerState()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		AWPS = PlayerController->GetPlayerState<AGamePlayerState>();

		if (AWPS)
		{
			GetWorld()->GetTimerManager().ClearTimer(ErrorTimerHandle);
			UpdateCharacter();  // UI 업데이트
		}
	}
}

void UWCharacterHUD::UpdateCharacter()
{
	// init 스탯
	SetState();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::SetState, 0.1f, true);
}

float UWCharacterHUD::GetHealthBarPercentage()
{
	if (AWPS == nullptr)
	{
		return 0.0f;
	}
		
	return AWPS->GetHP() / AWPS->GetMaxHP();
}

void UWCharacterHUD::ReBindSkill()
{
	AWCharacterBase* Character = Cast<AWCharacterBase>(GetOwningPlayerPawn());
	if (Character)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReBindSkillTimerHandle);
		Character->OnQSkillUsed.AddDynamic(this, &ThisClass::OnSkillUsed);
	}
}

void UWCharacterHUD::OnSkillUsed(float SkillCoolTime)
{
	QSkillCoolDownTime = SkillCoolTime;

	UsedQSkill();
	
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
	{		
		QSkillCoolDownTime -= 0.1;

		if (QSkillCoolDownTime <= 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
		}
	}, 0.1f, true);
}

void UWCharacterHUD::UsedQSkill_Implementation()
{
	// 블프에서 정의
}

void UWCharacterHUD::SetState()
{
	if (AWPS)
	{
		// 킬, 데스 출력
		FString KillString = FString::Printf(TEXT("K : %d"), AWPS->GetKillPoints());
		KillPoint->SetText(FText::FromString(KillString));
		
		FString DeathString = FString::Printf(TEXT("D : %d"), AWPS->GetDeathPoints());
		DeathPoint->SetText(FText::FromString(DeathString));
		
		// 캐릭터 스탯 출력
		FString PowerString = FString::Printf(TEXT("공격력: %d"), AWPS->CPower);
		Power->SetText(FText::FromString(PowerString));

		FString AHString = FString::Printf(TEXT("추가체력: %d"), AWPS->CAdditionalHealth);
		AdditionalHealth->SetText(FText::FromString(AHString));

		FString DefenceString = FString::Printf(TEXT("방어력: %.0f"), AWPS->CDefense);
		Defence->SetText(FText::FromString(DefenceString));

		FString SpeedString = FString::Printf(TEXT("스피드: %.1f"), AWPS->CSpeed/600);
		Speed->SetText(FText::FromString(SpeedString));

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