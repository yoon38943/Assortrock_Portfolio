#include "Character/UI/ItemStoreWidget.h"

#include "Character/WPlayerController.h"
#include "Character/WPlayerState.h"
#include "Components/TextBlock.h"

void UItemStoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PC = Cast<AWPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PS = Cast<AWPlayerState>(PC->PlayerState);
	}

	G_Attack = PS->Gold_Attack;
	G_Health = PS->Gold_Health;
	G_Defence = PS->Gold_Defence;
	G_Speed = PS->Gold_Speed;

	AttackGold->SetText(FText::FromString(FString::Printf(TEXT("%d Gold"), G_Attack)));
	HealthGold->SetText(FText::FromString(FString::Printf(TEXT("%d Gold"), G_Health)));
	DefenceGold->SetText(FText::FromString(FString::Printf(TEXT("%d Gold"), G_Defence)));
	SpeedGold->SetText(FText::FromString(FString::Printf(TEXT("%d Gold"), G_Speed)));
}

void UItemStoreWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PS->CGold < G_Attack)
	{
		AddAttack->SetIsEnabled(false);
	}
	else
	{
		AddAttack->SetIsEnabled(true);
	}
	if (PS->CGold < G_Health)
	{
		AddHealth->SetIsEnabled(false);
	}
	else
	{
		AddHealth->SetIsEnabled(true);
	}
	if (PS->CGold < G_Defence)
	{
		AddDefence->SetIsEnabled(false);
	}
	else
	{
		AddDefence->SetIsEnabled(true);
	}
	if (PS->CGold < G_Speed)
	{
		AddSpeed->SetIsEnabled(false);
	}
	else
	{
		AddSpeed->SetIsEnabled(true);
	}
	
}

void UItemStoreWidget::AddPowerState()
{
	if (PS)
	{
		PS->Server_AddGold(-G_Attack);
		PS->AddPower(10);
	}
}

void UItemStoreWidget::AddHealthState()
{
	if (PS)
	{
		PS->Server_AddGold(-G_Health);
		PS->AddHealth(200);
	}
}

void UItemStoreWidget::AddDefenceState()
{
	if (PS)
	{
		PS->Server_AddGold(-G_Defence);
		PS->AddDefence(10.f);
	}
}

void UItemStoreWidget::AddSpeedState()
{
	if (PS)
	{
		PS->Server_AddGold(-G_Speed);
		PS->AddSpeed(60);
	}
}