#include "Game/LobbyGameMode.h"

#include "OutGamePlayerState.h"
#include "ServerSessionPlayerController.h"
#include "WGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "Network/WGameSession.h"
#include "Network/WNetStatics.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = AWGameSession::StaticClass();
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 CurrentPlayersNum = GameState->PlayerArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("현재 세션 로그인 인원 : %d"), CurrentPlayersNum);

	int32 MaxPlayers = 0;
	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		FOnlineSessionSettings* Settings = SessionPtr->GetSessionSettings(NAME_GameSession);
		if (Settings)
		{
			MaxPlayers = Settings->NumPublicConnections;
		}
	}

	AssignTeamToPlayer(NewPlayer);

	AServerSessionPlayerController* Controller = Cast<AServerSessionPlayerController>(NewPlayer);
	if (Controller)
	{
		Controller->CloseDefaultLoadingScreen();

		Controller->ClientShowLoadingScreen(MaxPlayers);
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AServerSessionPlayerController* PC = Cast<AServerSessionPlayerController>(It->Get());
		if (PC)
		{
			PC->Client_UpdatePlayerCount(CurrentPlayersNum);
		}
	}

	if (CurrentPlayersNum == MaxPlayers)
	{
		StartGame();
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (IsServerTraveling)  // 예: ServerTravel 중에는 팀 삭제 안함
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerTravel 중 로그아웃 → 팀 정보 유지"));
		return;
	}
	
	UpdateTeamsInfoAfterAPlayerLeave(Cast<APlayerController>(Exiting));
	
	int32 CurrentPlayers = GameState->PlayerArray.Num();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AServerSessionPlayerController* PC = Cast<AServerSessionPlayerController>(It->Get());
		if (PC)
		{
			PC->Client_UpdatePlayerCount(CurrentPlayers);
		}
	}

	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (SessionPtr.IsValid())
	{
		FOnlineSessionSettings* CurrentSettings = SessionPtr->GetSessionSettings(NAME_GameSession);
		if (CurrentSettings)
		{
			SessionPtr->UpdateSession(NAME_GameSession, *CurrentSettings, true);
            
			UE_LOG(LogTemp, Warning, TEXT("Server: Player left. Updating session to Advertise again."));
		}
	}
}

void ALobbyGameMode::StartGame()
{
	UWGameInstance* GameInstance = Cast<UWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("세션이 가득찼으므로 게임 시작!"));
		UE_LOG(LogTemp, Warning, TEXT("비어 있는 플레이어 닉네임 부여!"));

		GameInstance->AssignPlayerNickName();

		GameInstance->FinalBlueTeamPlayersNum = BlueTeamNum;
		GameInstance->FinalRedTeamPlayersNum = RedTeamNum;
		GameInstance->LogFinalTeamNum();
		
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AServerSessionPlayerController* PC = Cast<AServerSessionPlayerController>(Iterator->Get());

			if (PC)
			{
				PC->Client_UpdateMatchingState(true);
			}
		}

		StartSelectCharacterMap();
	}
}

void ALobbyGameMode::StartSelectCharacterMap()
{
	UE_LOG(LogTemp, Warning, TEXT("캐릭터 선택 맵으로 이동 중..."));

	UWorld* World = GetWorld();
	if (World)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld() 성공, ServerTravel 시도"));
		
		// 로딩창 띄우기
		
		World->GetTimerManager().SetTimer(TravelTimerHandle, this, &ThisClass::ExecuteServerTravel, 2.5f, false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() == NULL!"));
	}
}

void ALobbyGameMode::ExecuteServerTravel()
{
	IsServerTraveling = true;
	FString LevelPath = NextInGameLevel.ToSoftObjectPath().GetLongPackageName();
	GetWorld()->ServerTravel(LevelPath, true);
}

void ALobbyGameMode::AssignTeamToPlayer(APlayerController* Player)
{
	AOutGamePlayerState* PS = Player->GetPlayerState<AOutGamePlayerState>();
	if (!PS) return;

	PS->PlayerInfo.PlayerName = PS->GetPlayerName();

	if (BlueTeamNum <= RedTeamNum)
	{
		PS->PlayerInfo.PlayerTeam = E_TeamID::Blue;
		PS->PlayerInfo.PlayerTeamID = BlueTeamNum;
		BlueTeamNum++;
		UE_LOG(LogTemp, Warning, TEXT("플레이어 %s -> 블루팀"), *PS->PlayerInfo.PlayerName);
	}
	else
	{
		PS->PlayerInfo.PlayerTeam = E_TeamID::Red;
		PS->PlayerInfo.PlayerTeamID = RedTeamNum;
		RedTeamNum++;
		UE_LOG(LogTemp, Warning, TEXT("플레이어 %s -> 레드팀"), *PS->PlayerInfo.PlayerName);
	}

	UE_LOG(LogTemp, Warning, TEXT("팀 상태 업데이트 - 블루팀 : %d / 레드팀 : %d"), BlueTeamNum, RedTeamNum);

	UWGameInstance* GameInstance = Cast<UWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("저장될 플레이어 이름 : %s"), *PS->PlayerInfo.PlayerName);
		GameInstance->SavePlayerTeamInfo(PS->PlayerInfo.PlayerName, PS->PlayerInfo);
	}
}

void ALobbyGameMode::UpdateTeamsInfoAfterAPlayerLeave(APlayerController* ExitingPlayer)
{
	if (!ExitingPlayer) return;

	AOutGamePlayerState* PS = ExitingPlayer->GetPlayerState<AOutGamePlayerState>();
	if (!PS) return;

	E_TeamID LeaveTeam = PS->PlayerInfo.PlayerTeam;
	if (LeaveTeam == E_TeamID::Blue)
	{
		BlueTeamNum--;
	}
	else if (LeaveTeam == E_TeamID::Red)
	{
		RedTeamNum--;
	}

	PS->PlayerInfo.PlayerTeam = E_TeamID::Neutral;
	PS->PlayerInfo.PlayerTeamID = -1;
	
	UE_LOG(LogTemp, Log, TEXT("플레이어 : %s (%s팀) 나감!"), *PS->GetPlayerName(), LeaveTeam == E_TeamID::Blue ? TEXT("블루") : TEXT("레드"));

	PS->PlayerInfo.PlayerTeam = E_TeamID::Neutral;

	UE_LOG(LogTemp, Log, TEXT("팀 상태 업데이트 - 블루팀 : %d / 레드팀 : %d"), BlueTeamNum, RedTeamNum);

	UWGameInstance* GameInstance = Cast<UWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->DeletePlayerTeamInfo(PS->PlayerInfo.PlayerName);
	}
}