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
	
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		AWPS = PlayerController->GetPlayerState<AWPlayerState>();

		if (AWPS)
		{
			auto Message = FString::Printf(TEXT("PlayerState 가져오기 성공: %s"), *AWPS->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
		}
		else
		{
			auto Message = FString::Printf(TEXT("PlayerState가 NULL. 0.5초 후 재시도."));
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
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
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, Message);
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