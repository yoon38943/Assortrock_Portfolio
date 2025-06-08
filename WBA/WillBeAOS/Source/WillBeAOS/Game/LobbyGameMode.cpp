#include "Game/LobbyGameMode.h"

#include "OutGamePlayerState.h"
#include "ServerSessionPlayerController.h"
#include "WGameInstance.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 CurrentPlayersNum = GameState->PlayerArray.Num();
	UE_LOG(LogTemp, Log, TEXT("현재 세션 로그인 인원 : %d"), CurrentPlayersNum);

	AssignTeamToPlayer(NewPlayer);

	AServerSessionPlayerController* PlayerController = Cast<AServerSessionPlayerController>(NewPlayer);
	if (PlayerController)
	{
		PlayerController->ClientShowLoadingScreen();
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AServerSessionPlayerController* PC = Cast<AServerSessionPlayerController>(It->Get());
		if (PC)
		{
			PC->Client_UpdatePlayerCount(CurrentPlayersNum);
		}
	}

	UWGameInstance* GameInstance = Cast<UWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->IsSessionFull())
		{
			UE_LOG(LogTemp, Log, TEXT("세션이 가득찼으므로 게임 시작!"));
			UE_LOG(LogTemp, Log, TEXT("비어 있는 플레이어 닉네임 부여!"));

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
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (IsServerTraveling)  // 예: ServerTravel 중에는 팀 삭제 안함
	{
		UE_LOG(LogTemp, Log, TEXT("ServerTravel 중 로그아웃 → 팀 정보 유지"));
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
}

void ALobbyGameMode::StartSelectCharacterMap()
{
	UE_LOG(LogTemp, Log, TEXT("캐릭터 선택 맵으로 이동 중..."));

	UWorld* World = GetWorld();
	if (World)
	{
		UE_LOG(LogTemp, Log, TEXT("GetWorld() 성공, ServerTravel 시도"));
		
		IsServerTraveling = true;
		World->ServerTravel("/Game/Portfolio/Menu/L_SelectCharacter?listen", true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() == NULL!"));
	}
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
		UE_LOG(LogTemp, Log, TEXT("플레이어 %s -> 블루팀"), *PS->PlayerInfo.PlayerName);
	}
	else
	{
		PS->PlayerInfo.PlayerTeam = E_TeamID::Red;
		PS->PlayerInfo.PlayerTeamID = RedTeamNum;
		RedTeamNum++;
		UE_LOG(LogTemp, Log, TEXT("플레이어 %s -> 레드팀"), *PS->PlayerInfo.PlayerName);
	}

	UE_LOG(LogTemp, Log, TEXT("팀 상태 업데이트 - 블루팀 : %d / 레드팀 : %d"), BlueTeamNum, RedTeamNum);

	UWGameInstance* GameInstance = Cast<UWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("저장될 플레이어 이름 : %s"), *PS->PlayerInfo.PlayerName);
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
	PS->PlayerInfo.PlayerTeamID = NULL;
	
	UE_LOG(LogTemp, Log, TEXT("플레이어 : %s (%s팀) 나감!"), *PS->GetPlayerName(), LeaveTeam == E_TeamID::Blue ? TEXT("블루") : TEXT("레드"));

	PS->PlayerInfo.PlayerTeam = E_TeamID::Neutral;

	UE_LOG(LogTemp, Log, TEXT("팀 상태 업데이트 - 블루팀 : %d / 레드팀 : %d"), BlueTeamNum, RedTeamNum);

	UWGameInstance* GameInstance = Cast<UWGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->DeletePlayerTeamInfo(PS->PlayerInfo.PlayerName);
	}
}