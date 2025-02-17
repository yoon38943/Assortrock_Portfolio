#include "WPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "WCharacterHUD.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/TowerNexusHPWidget.h"

void AWPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

	if (IsLocalController() && GameStateClass && UserWidgetClass)
	{
		// 월드 게임 플레이 관련
		if (!IsValid(GamePlayHUD))
		{
			GamePlayHUD = CreateWidget<UTowerNexusHPWidget>(this, GameStateClass);

			if (GamePlayHUD)
				GamePlayHUD->AddToViewport();
		}

		// 유저 위젯 관련
		if (!PlayerHUD)
		{
			PlayerHUD = CreateWidget<UWCharacterHUD>(this, UserWidgetClass);
			if (PlayerHUD)
				PlayerHUD->AddToViewport();
		}
	}
}

void AWPlayerController::OnPossess(APawn* InPawn)
{	
	Super::OnPossess(InPawn);
}


void AWPlayerController::OnGameStateChanged(E_GamePlay CurrentGameState)
{
	switch (CurrentGameState)
	{
	case E_GamePlay::GameInit:
		DisableInput(this);
		break;
    
	case E_GamePlay::ReadyCountdown:
		DisableInput(this);
		// 카운트다운 UI 표시
		break;
    
	case E_GamePlay::Gameplaying:
		EnableInput(this);
		// HUD 업데이트
		break;
    
	case E_GamePlay::GameEnded:
		GameHasEnded();
		// 결과 화면 표시
		break;
    
	default:
		break;
	}
}


void AWPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	if (GamePlayHUD)
		GamePlayHUD->RemoveFromParent();
	
	if(PlayerHUD)
		PlayerHUD->RemoveFromParent();

	if (bIsWinner)
	{
		SetShowMouseCursor(true);
		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen != nullptr)
		{
			WinScreen->AddToViewport();
		}
	}
	else
	{
		SetShowMouseCursor(true);
		UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
		if (LoseScreen != nullptr)
		{
			LoseScreen->AddToViewport();
		}
	}
}

void AWPlayerController::ShowRespawnWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	
	if (!RespawnScreen || !RespawnScreen->IsInViewport())
	{
		RespawnScreen = CreateWidget(GetWorld(), RespawnScreenClass);
		if (RespawnScreen)
		{
			RespawnScreen->AddToViewport(1); // ZOrder 조정
		}
	}

	AWGameState* GameState = Cast<AWGameState>(GetWorld()->GetGameState());
	CurrentRespawnTime = GameState->RespawnTime;
	
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::UpdateRespawnWidget, 1.f, true);
}

void AWPlayerController::UpdateRespawnWidget()
{
	if (!IsLocalController())
	{
		return;
	}
	
	if (CurrentRespawnTime > 1)
	{
		CurrentRespawnTime--;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
		HideRespawnWidget();
	}
}

void AWPlayerController::HideRespawnWidget()
{
	if (RespawnScreen != nullptr)
	{
		RespawnScreen->RemoveFromParent();
		RespawnScreen = nullptr;
	}
}

void AWPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}