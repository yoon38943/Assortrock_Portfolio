#include "WGameInstance.h"
#include "Character/WPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Net/UnrealNetwork.h"


void UWGameInstance::Init()
{
	Super::Init();

	Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		OnlineSessionInterface = Subsystem->GetSessionInterface();
		if (OnlineSessionInterface.IsValid())
		{
			OnlineSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindSessionComplete);
			OnlineSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionComplete);
			
			if (IsRunningDedicatedServer())
			{
				UE_LOG(LogTemp, Log, TEXT("서버 시작! 세션 생성 시작"));
			
				// 델리게이트 연결
				OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
			
				CreateGameSession();
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("클라이언트로 게임 시작!"));
			}
		}
	}
}

void UWGameInstance::LogFinalTeamNum()
{
	UE_LOG(LogTemp, Warning, TEXT("게임 인스턴스 - Blue : %d, Red : %d"), FinalBlueTeamPlayersNum, FinalRedTeamPlayersNum);
}

void UWGameInstance::SavePlayerTeamInfo(FString& PlayerNameInfo, FPlayerInfoStruct PlayerInfo)
{
	MatchPlayersTeamInfo.Add(*PlayerNameInfo, PlayerInfo);
	UE_LOG(LogTemp, Warning, TEXT("저장된 플레이어: %s, 팀 : %s, 팀 ID: %d"), *PlayerNameInfo, PlayerInfo.PlayerTeam == E_TeamID::Blue ? TEXT("블루팀") : TEXT("레드팀"), PlayerInfo.PlayerTeamID);
}

void UWGameInstance::DeletePlayerTeamInfo(FString PlayerName)
{
	SortPlayerTeamInfo(PlayerName);
	MatchPlayersTeamInfo.Remove(PlayerName);
}

void UWGameInstance::SortPlayerTeamInfo(FString ExitingPlayerName)
{
	if (MatchPlayersTeamInfo.Contains(ExitingPlayerName))
	{
		int32 ExitingPlayerTeamID = MatchPlayersTeamInfo.Find(ExitingPlayerName)->PlayerTeamID;

		for (TPair<FString, FPlayerInfoStruct>& PlayerInfo : MatchPlayersTeamInfo)
		{
			if (PlayerInfo.Value.PlayerTeamID > ExitingPlayerTeamID)
			{
				PlayerInfo.Value.PlayerTeamID -= 1;
			}
		}
	}
}

void UWGameInstance::SaveMatchPlayerTeam(FString PlayerNameInfo, E_TeamID TeamID, TSubclassOf<class APawn> PawnClass)
{
	if (!PlayerNameInfo.IsEmpty())
	{
		MatchPlayerTeams.Add(PlayerNameInfo, FPlayerValue(TeamID,false,PawnClass));
		UE_LOG(LogTemp, Warning, TEXT("🔹 저장된 플레이어: %s, 팀 ID: %d, 폰 클래스: %s"), *PlayerNameInfo, TeamID, *PawnClass->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerName is Null"));
	}
}

TMap<FString, FPlayerInfoStruct> UWGameInstance::GetSavedPlayerTeamInfo()
{
	return MatchPlayersTeamInfo;
}

TMap <FString, FPlayerValue> UWGameInstance::GetMatchTeam()
{
	return MatchPlayerTeams;
}

bool UWGameInstance::IsSessionFull()
{
	if (OnlineSessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
		if (Session)
		{
			int32 CurrentPlayers = Session->RegisteredPlayers.Num();
			int32 MaxPlayers = Session->SessionSettings.NumPublicConnections;

			UE_LOG(LogTemp, Log, TEXT("현재 세션 인원 : %d / %d"), CurrentPlayers, MaxPlayers);
			return CurrentPlayers >= MaxPlayers;
		}
	}
	
	return false;
}

void UWGameInstance::FindSessions()
{
	if (!OnlineSessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10;

	LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

	UE_LOG(LogTemp, Log, TEXT("세션 검색 중..."));
}

void UWGameInstance::OnFindSessionComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("클라: 세션 찾음! Join 시도"));

		OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[0]);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("클라: 세션 찾기 실패"));
	}
}

void UWGameInstance::StartJoinSession()
{
	if (OnlineSessionInterface.IsValid())
	{
		FindSessions();
	}
}

void UWGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectString;
		if (OnlineSessionInterface->GetResolvedConnectString(SessionName, ConnectString))
		{
			APlayerController* PC = GetFirstLocalPlayerController();
			if (PC)
			{
				PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
				UE_LOG(LogTemp, Log, TEXT("클라: 서버로 이동!"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("클라: 세션 Join 실패"));
	}
}

void UWGameInstance::CreateGameSession()
{
	if (OnlineSessionInterface.IsValid())
	{
		auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			OnlineSessionInterface->DestroySession(NAME_GameSession);
		}
		
		TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
		SessionSettings->bIsLANMatch = false;
		SessionSettings->NumPublicConnections = 2;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bAllowJoinViaPresence = true;
		SessionSettings->bShouldAdvertise = true;
		SessionSettings->bUsesPresence = true;

		UE_LOG(LogTemp, Log, TEXT("서버 : 세션 생성 중..."));
		OnlineSessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings);
	}
	else
	{
		return;
	}
}

void UWGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Cyan,
				FString::Printf(TEXT("서버 : 세션 생성 성공! : %s"),*SessionName.ToString()));
		}
		UE_LOG(LogTemp, Log, TEXT("서버 : 세션 생성 성공! : %s"), *SessionName.ToString());
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Red,
				FString::Printf(TEXT("세션 생성 실패!")));
		}
		UE_LOG(LogTemp, Log, TEXT("서버 : 세션 생성 실패!"));
	}
}

void UWGameInstance::LeaveGameSession_Implementation()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->ClientTravel("/Game/Portfolio/Menu/L_MainLobby", ETravelType::TRAVEL_Absolute);
		UE_LOG(LogTemp, Log, TEXT("세션 떠나기 완료!"));
	}

	// 클라 세션 파괴
	if (OnlineSessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
		if (Session)
		{
			OnlineSessionInterface->DestroySession(NAME_GameSession);
			UE_LOG(LogTemp, Log, TEXT("클라: 세션 Destroy 요청!"));
		}
	}
}


void UWGameInstance::DestroySession()
{
	if (OnlineSessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
		if (Session)
		{
			OnlineSessionInterface->DestroySession(NAME_GameSession);
			UE_LOG(LogTemp, Log, TEXT("마지막 참가자라 세션을 파괴하면서 나감"));

			bSessionLeaveBeforeCreateSession = false;
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					10.f,
					FColor::Cyan,
					FString::Printf(TEXT("세션 파괴!")));
			}
		}
		else
		{
			bSessionLeaveBeforeCreateSession = true;
			UE_LOG(LogTemp, Log, TEXT("세션이 없으므로 파괴 요청 생략!"));
		}
	}
}

void UWGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Log, TEXT("%s 파괴 성공!"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s 파괴 실패!"), *SessionName.ToString());
	}
}