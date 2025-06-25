#include "Character/UI/TowerNexusHPWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Game/WGameState.h"

void UTowerNexusHPWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AWGS = GetWorld()->GetGameState<AWGameState>();
	if (AWGS)
	{
		UpdateTeamKillPoints(AWGS->BlueTeamTotalKillPoints, AWGS->RedTeamTotalKillPoints);
		AWGS->DelegateShowKillState.BindUObject(this, &ThisClass::UpdateTeamKillPoints);
	}
}

float UTowerNexusHPWidget::SetBlueTeamNexusHealth()
{
	if (AWGS)
	{
		return AWGS->GetBlueNexusHP();
	}
	return 0.0f;
}

float UTowerNexusHPWidget::SetRedTeamNexusHealth()
{
	if (AWGS)
	{
		return AWGS->GetRedNexusHP();
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

void UTowerNexusHPWidget::UpdateTeamKillPoints(int32 Blue, int32 Red)
{
	FString BlueTeam = FString::Printf(TEXT("%d"), Blue);
	BlueTeamKillPoints->SetText(FText::FromString(BlueTeam));

	FString RedTeam = FString::Printf(TEXT("%d"), Red);
	RedTeamKillPoints->SetText(FText::FromString(RedTeam));
}
