#include "OutPlayerController.h"
#include "OutGameMode.h"
#include "OutGameState.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AOutPlayerController::AOutPlayerController()
{
	SetShowMouseCursor(true);
}

void AOutPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("PlayerController BeginPlay."));

	
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

	AOutGameState* GameState = Cast<AOutGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		GameState->PlayerControllers.Add(this);\
		UE_LOG(LogTemp, Warning, TEXT("GamestatePlayerControllers Add %s."), *GetName());
	}
}