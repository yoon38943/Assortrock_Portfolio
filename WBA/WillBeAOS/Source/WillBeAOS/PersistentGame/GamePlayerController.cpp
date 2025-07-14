#include "PersistentGame/GamePlayerController.h"

#include "GamePlayerState.h"
#include "PlayGameMode.h"
#include "PlayGameState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/WCharacterBase.h"
#include "Character/UI/SelectMapUserWidget.h"
#include "Gimmick/Tower.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/UI/TowerNexusHPWidget.h"
#include "../Character/WCharacterHUD.h"
#include "Gimmick/PlayerSpawner.h"
#include "Net/UnrealNetwork.h"


void AGamePlayerController::StartCharacterSelectPhase()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AGamePlayerController::Server_ControllerIsReady_Implementation()
{
	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->CheckPlayerIsReady(this);
	}
}

void AGamePlayerController::PlayerStateInfoReady_Implementation()
{
	SelectCharacterWidget = CreateWidget<UUserWidget>(this, SelectCharacterWidgetClass);
	if (SelectCharacterWidget)
	{
		SelectCharacterWidget->AddToViewport(0);
	}

	// 위젯까지 띄운 후에 준비 클라 준비 완료됐다고 보고하기
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 준비 완료"));
	Server_ControllerIsReady();
}

void AGamePlayerController::UpdatePlayerWidget_Implementation()
{
	USelectMapUserWidget* Widget = Cast<USelectMapUserWidget>(SelectCharacterWidget);
	if (Widget)
	{
		Widget->UpdateWidget();
	}
}

void AGamePlayerController::BackToLobby_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 게임 로비로 복귀 중..."));
	ClientTravel("/Game/Portfolio/Menu/L_MainLobby", TRAVEL_Absolute);
}

void AGamePlayerController::ToInGameLoading_Implementation()
{
	if (SelectCharacterWidget && SelectCharacterWidget->IsInViewport())
	{
		SelectCharacterWidget->RemoveFromParent();
		SelectCharacterWidget = nullptr;
	}
	
	LoadingWidget = CreateWidget<UUserWidget>(this, ToInGameLoadingWidgetClass);
	if (LoadingWidget)
	{
		LoadingWidget->AddToViewport(0);
	}
}

void AGamePlayerController::CheckLoadedAllStreamingLevels()
{
	FName LastLevelName;
	
	APlayGameMode* GM = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		LastLevelName = FName(GM->StreamingLevelSequence.Last().GetAssetName());
	}

	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(this, LastLevelName);
	if (StreamingLevel && StreamingLevel->IsLevelLoaded() && StreamingLevel->IsLevelVisible())
	{
		// 레벨이 로드 됐으면 클라 준비 완료
		StartInGamePhase();		// 인게임 컨트롤러 실행
		
		AGamePlayerState* PS = GetPlayerState<AGamePlayerState>();
		if (PS)
		{
			PS->StartInGamePhase();		// 인게임 플레이어 스테이트 실행
		}
	}
	else
	{
		// 레벨이 클라에서 로드 되었는지 재확인
		FTimerHandle ReCheckTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ReCheckTimerHandle, this, &ThisClass::CheckLoadedAllStreamingLevels, 0.1f, false);
	}
}

void AGamePlayerController::StartInGamePhase()
{
	// 인게임 전환 로딩창 비활성화
	if (LoadingWidget && LoadingWidget->IsInViewport())
	{
		LoadingWidget->RemoveFromParent();
		LoadingWidget = nullptr;
	}

	// 다시 마우스 안보이도록
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	
	// 캐릭터 소환되면 인게임 위젯 붙이기
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

		// 플레이어 준비 됐다고 신호 보내기
		Server_SetPlayerReady();
	}
}

void AGamePlayerController::BP_StartInGamePhase_Implementation()
{
	// 블프 내에서 구현
}

void AGamePlayerController::HiddenRecallWidget_Implementation(bool IsRecallCompleted)
{
	if (IsRecallCompleted)
	{
		UE_LOG(LogTemp, Warning, TEXT("귀환 성공!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("귀환 취소됨!"));
	}
	
	if (RecallWidget && RecallWidget->IsInViewport())
	{
		RecallWidget->RemoveFromParent();
		RecallWidget = nullptr;
	}
}

void AGamePlayerController::ShowRecallWidget_Implementation()
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

void AGamePlayerController::StartRecall_Implementation()
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

void AGamePlayerController::SetIsOpenStore_Implementation(bool CanOpen)
{
	IsOpenedStore = CanOpen;
}

void AGamePlayerController::Server_CancelRecall_Implementation()
{
	CancelRecall();
}

void AGamePlayerController::CancelRecall()
{
	if (!IsRecalling) return;

	IsRecalling = false;
	if (RecallTimerHandle.IsValid())
		GetWorldTimerManager().ClearTimer(RecallTimerHandle);

	HiddenRecallWidget(false);

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		PlayerChar->NM_StopPlayMontage();
	}
}

void AGamePlayerController::CompleteRecall()
{
	if (!IsRecalling) return;

	IsRecalling = false;

	HiddenRecallWidget(true);
	
	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetPawn());
	if (PlayerChar)
	{
		PlayerChar->ServerPlayMontage(PlayerChar->CompleteRecallMontage);
	}
	
	RecallToBase();
	
	GetWorldTimerManager().ClearTimer(RecallTimerHandle);
}

void AGamePlayerController::RecallToBase()
{
	if (AGamePlayerState* WPlayerState = GetPlayerState<AGamePlayerState>())
	{
		if (AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(GetCharacter()))
		{
			PlayerChar->SetActorLocation(WPlayerState->PlayerSpawner->GetActorLocation());
			SetControlRotation(FRotationMatrix::MakeFromX(FVector(0, 0, 100) - GetPawn()->GetActorLocation()).Rotator());
		}
	}
}

void AGamePlayerController::S_SetCurrentRespawnTime_Implementation()
{
	if (APlayGameState* GameState = Cast<APlayGameState>(GetWorld()->GetGameState()))
	{
		CurrentRespawnTime = GameState->RespawnTime;
		C_ReplicateCurrentRespawnTime(CurrentRespawnTime);
	}
}

void AGamePlayerController::GameEnded_Implementation(E_TeamID LoseTeam)
{
	if (!IsLocalController()) return;
	
	if (GamePlayHUD)
		GamePlayHUD->RemoveFromParent();
	
	if(PlayerHUD)
		PlayerHUD->RemoveFromParent();

	AGamePlayerState* PS = GetPlayerState<AGamePlayerState>();
	if (PS)
	{
		PlayerTeamID = PS->InGamePlayerInfo.PlayerTeam;
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

void AGamePlayerController::ShowRespawnWidget()
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
}

void AGamePlayerController::UpdateRespawnWidget()
{
	if (CurrentRespawnTime > 1)
	{
		CurrentRespawnTime--;
		C_ReplicateCurrentRespawnTime(CurrentRespawnTime);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
		HideRespawnWidget();
	}
}

void AGamePlayerController::HideRespawnWidget_Implementation()
{
	if (RespawnScreen != nullptr)
	{
		RespawnScreen->RemoveFromParent();
		RespawnScreen = nullptr;
	}
}

void AGamePlayerController::C_ReplicateCurrentRespawnTime_Implementation(int32 RespawnTime)
{
	CurrentRespawnTime = RespawnTime;
}

void AGamePlayerController::S_CountRespawnTime_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::UpdateRespawnWidget, 1.f, true);
}

void AGamePlayerController::PossessToSpectatorCamera(FVector CameraLocation, FRotator CameraRotation)
{
	FActorSpawnParameters SpawnParams;
	APawn* DeadCamera = GetWorld()->SpawnActor<APawn>(SpectorCamera, CameraLocation, CameraRotation, SpawnParams);

	if (DeadCamera)
	{
		Possess(DeadCamera);
	}
}

void AGamePlayerController::OnGameStateChanged(E_GamePlay CurrentGameState)
{
	switch (CurrentGameState)
	{
	case E_GamePlay::ReadyCountdown:
		DisableInput(this);
		break;
    
	case E_GamePlay::Gameplaying:
		EnableInput(this);
		break;
    
	case E_GamePlay::GameEnded:
		GameHasEnded();
		// 결과 화면 표시
		break;
    
	default:
		break;
	}
}

void AGamePlayerController::Server_SetPlayerReady_Implementation()
{
	AGamePlayerState* PS = GetPlayerState<AGamePlayerState>();
	if (PS)
	{
		PS->S_SetPlayerReady(true); // 플레이어 상태 변경
	}
}

void AGamePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(InPawn);
	if (PlayerChar)
	{
		if (PlayerChar)
		{
			if (AGamePlayerState* WPlayerState = GetPlayerState<AGamePlayerState>())
				PlayerChar->TeamID = WPlayerState->InGamePlayerInfo.PlayerTeam;
			UE_LOG(LogTemp, Log, TEXT("AWPlayerController::OnPossess %d"),PlayerChar->TeamID);
		}

		/*// 타워에 캐릭터 재설정 부분
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
		}*/
	}
}

void AGamePlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CountdownTime);
	DOREPLIFETIME(ThisClass, IsRecalling);
}