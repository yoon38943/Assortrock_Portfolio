#include "Character/UI/TowerNexusHPWidget.h"

#include "Components/ProgressBar.h"
#include "Game/WGameState.h"

void UTowerNexusHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AWGS = GetWorld()->GetGameState<AWGameState>();
}

float UTowerNexusHPWidget::SetTowerProgress()
{
	if (AWGS != nullptr)
	{
		if (AWGS->GetTowerNum() == 0)
		{
			FriendTowerProgress->SetVisibility(ESlateVisibility::Hidden);
		}
		return (AWGS->GetTowerNum()/6.0f); // 6.0f -> 타워 MAX_NUM으로 변경
	}
	return 0.5f;
}

float UTowerNexusHPWidget::SetNexusHealth()
{
	if (AWGS != nullptr)
	{
		return AWGS->GetNexusHP();
	}
	return 0.0f;
}

FText UTowerNexusHPWidget::UpdateGameTimer()
{
	float CurrentGameTime = AWGS->GetServerWorldTimeSeconds();
	float OneGametime = 45 * 60;
	float RestGameTime = OneGametime - CurrentGameTime;

	FString TimeString = FString::Printf(TEXT("%d : %02d"), static_cast<int32>(RestGameTime/60) , static_cast<int32>(FMath::Fmod(RestGameTime, 60)));
	return FText::FromString(TimeString);
}