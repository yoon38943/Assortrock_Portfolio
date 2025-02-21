#include "Character/UI/ItemStoreWidget.h"

#include "Character/WPlayerController.h"
#include "Character/WPlayerState.h"

void UItemStoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PC = Cast<AWPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PS = Cast<AWPlayerState>(PC->PlayerState);
	}
}

void UItemStoreWidget::AddPowerState()
{
	if (PS)
	{
		PS->AddPower(10);
	}
}

void UItemStoreWidget::AddHealthState()
{
	if (PS)
	{
		PS->AddHealth(200);
	}
}

void UItemStoreWidget::AddDefenceState()
{
	if (PS)
	{
		PS->AddDefence(10.f);
	}
}

void UItemStoreWidget::AddSpeedState()
{
	if (PS)
	{
		PS->AddSpeed(60);
	}
}
