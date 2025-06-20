#include "WPlayerController.h"
#include "WCharacterBase.h"
#include "Blueprint/UserWidget.h"
#include "WCharacterHUD.h"
#include "WPlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Gimmick/PlayerSpawner.h"
#include "Gimmick/Tower.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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

		Server_SetPlayerReady();
	}
}

void AWPlayerController::Server_SetPlayerReady_Implementation()
{
	AWPlayerState* PS = GetPlayerState<AWPlayerState>();
	if (PS)
	{
		PS->S_SetPlayerReady(true); // 플레이어 상태 변경
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
			PlayerChar->TeamID = WPlayerState->PlayerInfo.PlayerTeam;
			UE_LOG(LogTemp, Log, TEXT("AWPlayerController::OnPossess %d"),PlayerChar->TeamID);
		}

		if (this == GetWorld()->GetFirstPlayerController())
		{
			TArray<AActor*> FoundTowers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), FoundTowers);

			for (AActor* Actor : FoundTowers)
			{
				ATower* Tower = Cast<ATower>(Actor);
				if (Tower)
				{
					Tower->FindPlayerPawn();  // 타워의 특정 함수 호출
				}
			}
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

void AWPlayerController::OnRep_Countdown()
{	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("GameCountdown %d"),CountdownTime));
}

void AWPlayerController::StartRecall_Implementation()
{
	if (IsRecalling) return;

	IsRecalling = true;

	ShowRecallWidget();

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

void AWPlayerController::ShowRecallWidget_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("귀환 시작!"));

	if (!RecallWidget)
	{
		RecallWidget = CreateWidget<UUserWidget>(this, RecallWidgetClass);
		if (RecallWidget)
		{
			RecallWidget->AddToViewport();
		}
	}
}

void AWPlayerController::HiddenRecallWidget_Implementation(bool IsRecallCancel)
{
	if (IsRecallCancel)
	{
		UE_LOG(LogTemp, Warning, TEXT("귀환 취소됨!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("귀환 성공!"));
	}
	
	if (RecallWidget && RecallWidget->IsInViewport())
	{
		RecallWidget->RemoveFromParent();
		RecallWidget = nullptr;
	}
}

void AWPlayerController::Server_CancelRecall_Implementation()
{
	CancelRecall();
}

void AWPlayerController::CancelRecall()
{
	if (!IsRecalling) return;

	IsRecalling = false;
	if (RecallTimerHandle.IsValid())
		GetWorldTimerManager().ClearTimer(RecallTimerHandle);

	HiddenRecallWidget(true);

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		PlayerChar->NM_StopPlayMontage();
	}
}

void AWPlayerController::CompleteRecall()
{
	if (!IsRecalling) return;

	IsRecalling = false;

	HiddenRecallWidget(false);
	
	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		PlayerChar->ServerPlayMontage(PlayerChar->CompleteRecallMontage);
	}
	
	RecallToBase();
	
	GetWorldTimerManager().ClearTimer(RecallTimerHandle);
}

void AWPlayerController::RecallToBase()
{
	if (AWPlayerState* WPlayerState = GetPlayerState<AWPlayerState>())
	{
		if (AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetCharacter()))
		{
			PlayerChar->SetActorLocation(WPlayerState->PlayerSpawner->GetActorLocation());
			SetControlRotation(FRotationMatrix::MakeFromX(FVector(0, 0, 100) - GetPawn()->GetActorLocation()).Rotator());
		}
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
		PlayerTeamID = PS->PlayerInfo.PlayerTeam;
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

void AWPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CountdownTime);
	DOREPLIFETIME(ThisClass, IsRecalling);
}