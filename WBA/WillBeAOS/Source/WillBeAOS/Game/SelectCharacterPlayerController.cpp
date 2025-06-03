#include "Game/SelectCharacterPlayerController.h"

#include "Blueprint/UserWidget.h"

void ASelectCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority()) return;
	
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	UUserWidget* SelectCharacterWidget = CreateWidget<UUserWidget>(this, SelectCharacterWidgetClass);
	if (SelectCharacterWidget)
	{
		SelectCharacterWidget->AddToViewport(0);
	}
}
