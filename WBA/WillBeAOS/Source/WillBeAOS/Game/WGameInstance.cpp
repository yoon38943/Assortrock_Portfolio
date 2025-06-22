#include "WGameInstance.h"
#include "Character/WPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"

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
				IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
				if (OnlineSub)
				{
					IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						ELoginStatus::Type Status = IdentityInterface->GetLoginStatus(0);
						UE_LOG(LogTemp, Log, TEXT("서버 Steam 로그인 상태: %d"), (int32)Status);
					}
				}
                
				UE_LOG(LogTemp, Log, TEXT("서버 시작! 세션 생성 시작"));
			
				// 델리게이트 연결
				OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnCreateSessionComplete);
			
				CreateGameSession();
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("클라이언트로 게임 시작!"));
				
				IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
				if (OnlineSub)
				{
					IOnlineIdentityPtr IdentityInterface = OnlineSub->GetIdentityInterface();
					if (IdentityInterface.IsValid())
					{
						ELoginStatus::Type Status = IdentityInterface->GetLoginStatus(0);
						UE_LOG(LogTemp, Log, TEXT("클라 Steam 로그인 상태: %d"), (int32)Status);
					}
				}
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

TMap<FString, FPlayerInfoStruct> UWGameInstance::GetSavedPlayerTeamInfo()
{
	return MatchPlayersTeamInfo;
}

void UWGameInstance::AssignPlayerNickName()
{
	for (TPair<FString, FPlayerInfoStruct>& PlayerInfo : MatchPlayersTeamInfo)
	{
		if (PlayerInfo.Value.PlayerNickName.IsEmpty())
		{
			PlayerInfo.Value.PlayerNickName = FString::Printf(TEXT("%s팀 유저 %d"), PlayerInfo.Value.PlayerTeam == E_TeamID::Blue ? TEXT("블루") : TEXT("레드"), PlayerInfo.Value.PlayerTeamID + 1);
		}
	}
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
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10000;

	SessionSearch->QuerySettings.Set(FName("AOSPortfolio"), FString("MyAOSGame"), EOnlineComparisonOp::Equals);
	
	OnlineSessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	

	UE_LOG(LogTemp, Log, TEXT("세션 검색 중..."));
}

void UWGameInstance::OnFindSessionComplete(bool Success)
{
	if (!OnlineSessionInterface.IsValid() || !Success)
	{
		UE_LOG(LogTemp, Log, TEXT("클라: 세션 찾기 실패"));
		OnFindSessionFailed.Broadcast();
		return;
	}
	    
	for (auto Result : SessionSearch->SearchResults)
	{
		for (const TPair<FName, FOnlineSessionSetting>& SettingPair : Result.Session.SessionSettings.Settings)
		{
			const FName& Key = SettingPair.Key;
			const FOnlineSessionSetting& Setting = SettingPair.Value;

			UE_LOG(LogTemp, Log, TEXT("Key: %s, Value: %s"), *Key.ToString(), *Setting.Data.ToString());
		}
        
		FString SessionValue;
		Result.Session.SessionSettings.Get(FName("AOSPortfolio"), SessionValue);

		if (SessionValue == FString("MyAOSGame"))
		{
			UE_LOG(LogTemp, Log, TEXT("찾은 세션 Key값에 대한 Value : %s"), *SessionValue);

			OnFindSessionSuccess.Broadcast();
			OnlineSessionInterface->JoinSession(0, NAME_GameSession, Result);
			return;
		}
	}

	OnFindSessionFailed.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("클라: 모든 검색 세션 경유"));
}

void UWGameInstance::StartJoinSession()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS)
	{
		IOnlineSessionPtr Sessions = OSS->GetSessionInterface();
		if (Sessions.IsValid() && Sessions->GetNamedSession(NAME_GameSession) != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("남아 있던 클라 세션 파괴!"));
			Sessions->DestroySession(NAME_GameSession);
		}
	}
	
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
				UE_LOG(LogTemp, Log, TEXT("클라: 서버로 이동!"));
				UE_LOG(LogTemp, Log, TEXT("%s"), *SessionName.ToString());
				UE_LOG(LogTemp, Log, TEXT("%s"), *ConnectString);
				PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
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
		
		SessionSettings = MakeShareable(new FOnlineSessionSettings());
		SessionSettings->bIsLANMatch = true;
		SessionSettings->NumPublicConnections = 1;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bShouldAdvertise = true;
		SessionSettings->bUseLobbiesIfAvailable = true;
		SessionSettings->bIsDedicated = true;
		SessionSettings->bUsesPresence = false;
		SessionSettings->bAllowJoinViaPresence = false;

		SessionSettings->Set(FName("AOSPortfolio"), FString("MyAOSGame"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

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
		UE_LOG(LogTemp, Log, TEXT("서버 : 세션 생성 성공! : %s"), *SessionName.ToString());

		FString MyValue;
		if (SessionSettings->Get(FName("AOSPortfolio"), MyValue))
		{
			UE_LOG(LogTemp, Log, TEXT("서버에서 AOSPortfolio 등록 확인: %s"), *MyValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("서버에서 AOSPortfolio 값이 없다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("서버 : 세션 생성 실패!"));
	}
}

void UWGameInstance::LeaveGameSession_Implementation()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		if (OutLobbyMap.IsNull()) return;
        
		FString MainMap = OutLobbyMap.GetAssetName();
		UE_LOG(LogTemp, Log, TEXT("세션 떠나기 완료!"));
		PC->ClientTravel(MainMap, ETravelType::TRAVEL_Absolute);
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