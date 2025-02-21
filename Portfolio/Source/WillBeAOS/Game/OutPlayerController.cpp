#include "OutPlayerController.h"
#include "OutGameMode.h"
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

void AOutPlayerController::ServerSetReady_Implementation(bool bReady)
{
	if (AOutGameMode* GameMode = Cast<AOutGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->SetPlayerReady(this, bReady);
	}
}

bool AOutPlayerController::ServerSetReady_Validate(bool bReady)
{

	// 서버에서만 실행 가능하도록 설정
	AOutGameMode* GameMode = Cast<AOutGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerSetReady_Validate failed: GameMode is null."));
		return false; // GameMode가 없으면 요청 거부
	}

	// 중복 요청 방지 (이미 준비 상태인 경우 거부)
	if (GameMode->IsPlayerAlreadyReady(this) == bReady) 
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerSetReady_Validate: Duplicate request ignored."));
		return true; // 요청은 허용하지만 경고 로그 출력
	}
	
	return true;
}
