#include "OutPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AOutPlayerController::AOutPlayerController()
{
	SetShowMouseCursor(true);
}

void AOutPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() && MainMenuClass != nullptr)
	{
		MainMenu = CreateWidget<UUserWidget>(this, MainMenuClass);
		if (MainMenu)
		{
			MainMenu->AddToViewport(0);

			// 플레이어 컨트롤러의 입력모드를 UI 전용으로 설정. ( 마우스 커서가 뷰포트에 Lock 걸리지 않게 )
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, MainMenu, EMouseLockMode::DoNotLock);
		}
	}
}
