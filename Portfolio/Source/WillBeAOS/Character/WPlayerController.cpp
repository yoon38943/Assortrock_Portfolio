#include "WPlayerController.h"
#include "WCharacterBase.h"
#include "Blueprint/UserWidget.h"
#include "WCharacterHUD.h"
#include "WPlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Game/WGameMode.h"
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
	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(InPawn);
	if (PlayerChar)
	{
		if (PlayerChar)
		{
			if (AWPlayerState* WPlayerState = GetPlayerState<AWPlayerState>())
			PlayerChar->TeamID = WPlayerState->TeamID;
			UE_LOG(LogTemp, Log, TEXT("AWPlayerController::OnPossess %d"),PlayerChar->TeamID);
		}
	}
}


void AWPlayerController::OnGameStateChanged(E_GamePlay CurrentGameState)
{
	switch (CurrentGameState)
	{
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

void AWPlayerController::StartRecall()
{
	if (IsRecalling) return;

	IsRecalling = true;
	UE_LOG(LogTemp, Warning, TEXT("귀환 시작!"));

	if (!RecallWidget)
	{
		RecallWidget = CreateWidget<UUserWidget>(this, RecallWidgetClass);
		if (RecallWidget)
		{
			RecallWidget->AddToViewport();
		}
	}

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		if (PlayerChar)
		{
			PlayerChar->ServerPlayMontage(PlayerChar->StartRecallMontage);
		}
	}

	GetWorldTimerManager().SetTimer(RecallTimerHandle, this, &ThisClass::CompleteRecall, RecallTime, false);
}

void AWPlayerController::CancelRecall()
{
	if (!IsRecalling) return;

	IsRecalling = false;
	GetWorldTimerManager().ClearTimer(RecallTimerHandle);

	if (RecallWidget && RecallWidget->IsInViewport())
	{
		RecallWidget->RemoveFromParent();
		RecallWidget = nullptr;
	}

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		PlayerChar->StopAnimMontage();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("귀환 취소됨!"));
}

void AWPlayerController::CompleteRecall()
{
	if (!IsRecalling) return;

	IsRecalling = false;
	UE_LOG(LogTemp, Warning, TEXT("귀환 성공함"));
	
	if (RecallWidget && RecallWidget->IsInViewport())
	{
		RecallWidget->RemoveFromParent();
		RecallWidget = nullptr;
	}
	
	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		PlayerChar->ServerPlayMontage(PlayerChar->CompleteRecallMontage);
	}
	
	RecallToBase();
	SetControlRotation(FRotator(0, 0, 0));
	//SetActorRotation(FRotator(0, 0, 0));		// 캐릭터가 컨트롤러가 바라보는 방향에 묶여있음
	
	GetWorldTimerManager().ClearTimer(RecallTimerHandle);
}

void AWPlayerController::RecallToBase_Implementation()
{
	AWGameMode* GM = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		AActor* PlayerStart = GM->FindPlayerStart(this);
		AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetCharacter());
		PlayerChar->SetActorLocation(PlayerStart->GetActorLocation());
	}
}

void AWPlayerController::GameEnded_Implementation(E_TeamID LoseTeam)
{
	if (!IsLocalController()) return;
	
	if (GamePlayHUD)
		GamePlayHUD->RemoveFromParent();
	
	if(PlayerHUD)
		PlayerHUD->RemoveFromParent();

	AWPlayerState* PS = GetPlayerState<AWPlayerState>();
	if (PS)
	{
		PlayerTeamID = PS->TeamID;
	}
	
	if (PlayerTeamID != LoseTeam)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(nullptr);

		SetInputMode(InputMode);
		SetShowMouseCursor(true);
		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen != nullptr)
		{
			WinScreen->AddToViewport();
		}
	}
	else
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(nullptr);

		SetInputMode(InputMode);
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