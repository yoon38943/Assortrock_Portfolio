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
		PS->SetPower(10);
	}
}

void UItemStoreWidget::AddHealthState()
{
	if (PS)
	{
		PS->SetHealth(200);
	}
}
