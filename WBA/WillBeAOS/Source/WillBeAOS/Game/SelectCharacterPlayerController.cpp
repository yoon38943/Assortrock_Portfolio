#include "Game/SelectCharacterPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Character/UI/SelectMapUserWidget.h"

void ASelectCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority()) return;
	
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ASelectCharacterPlayerController::PlayerStateInfoReady_Implementation()
{
	SelectCharacterWidget = CreateWidget<UUserWidget>(this, SelectCharacterWidgetClass);
	if (SelectCharacterWidget)
	{
		SelectCharacterWidget->AddToViewport(0);
	}
}

void ASelectCharacterPlayerController::UpdatePlayerWidget_Implementation()
{
	USelectMapUserWidget* Widget = Cast<USelectMapUserWidget>(SelectCharacterWidget);
	if (Widget)
	{
		Widget->UpdateWidget();
	}
}