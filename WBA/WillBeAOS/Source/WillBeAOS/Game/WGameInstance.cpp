#include "WGameInstance.h"
#include "Character/WPlayerState.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Network/WNetStatics.h"

void UWGameInstance::Init()
{
	Super::Init();

	if (GetWorld()->IsEditorWorld()) return;

	if (UWNetStatics::IsSessionServer(this))
	{
		CreateSession();
	}
}

void UWGameInstance::StartMatchmaking()
{
	UE_LOG(LogTemp, Display, TEXT("매치메이킹 시작!!!"));

	FindSessions();
}

bool UWGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UWNetStatics::GetIdentityPtr())
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}

	return false;
}

bool UWGameInstance::IsLoggingIn() const
{
	return LoggingInDelegatedHandle.IsValid();
}

void UWGameInstance::ClientAccountPortalLogin()
{
	ClinetLogin("AccountPortal", "", "");
}

void UWGameInstance::ClinetLogin(const FString& Type, const FString& Id, const FString& Token)
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
    
	if (OSS)
	{
		// 2. 서브시스템의 진짜 이름을 로그로 찍습니다.
		FString SubsystemName = OSS->GetSubsystemName().ToString();
		UE_LOG(LogTemp, Warning, TEXT("#### 현재 활성화된 서브시스템: %s"), *SubsystemName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("#### 온라인 서브시스템을 찾을 수 없습니다!"));
	}
	
	if (IOnlineIdentityPtr IdentityPtr = UWNetStatics::GetIdentityPtr())
	{
		if (LoggingInDelegatedHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegatedHandle);
			LoggingInDelegatedHandle.Reset();
		}
		
		LoggingInDelegatedHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::LoginCompleted);
		
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed Right Away!!!"));
			if (LoggingInDelegatedHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegatedHandle);
				LoggingInDelegatedHandle.Reset();
			}
			OnLoginCompleted.Broadcast(false, "", "Login Failed Right Away!!!");
		}
	}
}

void UWGameInstance::LoginCompleted(int NumOfLocalPlayers, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error)
{
	if (IOnlineIdentityPtr IdentityPtr = UWNetStatics::GetIdentityPtr())
	{
		if (LoggingInDelegatedHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegatedHandle);
			LoggingInDelegatedHandle.Reset();
		}

		FString PlayerNickname = "";
		if (bWasSuccessful)
		{
			PlayerNickname = IdentityPtr->GetPlayerNickname(UserId);
			OnLoginCompleted.Broadcast(true, PlayerNickname, "");
			UE_LOG(LogTemp, Warning, TEXT("Logged in successfully as : %s"), *PlayerNickname);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed : %s"), *Error);
		}
	}
	else
	{
		OnLoginCompleted.Broadcast(false, "", "Can't find the Identity Pointer");
	}
}

void UWGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}
	
	PlayerRecord.Add(UniqueId);
}

void UWGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Left is empty"));
		TerminateSessionServer();
	}
}

void UWGameInstance::FindSessions()
{
	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch());

		SessionSearch->bIsLanQuery = false;
		SessionSearch->MaxSearchResults = 10000;
		SessionSearch->QuerySettings.Set(FName("PRESENCESEARCH"), true, EOnlineComparisonOp::Equals);

		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &ThisClass::OnFindeSessionsComplete);

		SessionPtr->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UWGameInstance::OnFindeSessionsComplete(bool bWasSuccessful)
{
	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (!SessionPtr) return;

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		int32 NumSessions = SessionSearch->SearchResults.Num();
		UE_LOG(LogTemp, Warning, TEXT("Found %d Sessions"), NumSessions);

		if (NumSessions > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("방을 찾았습니다. 참가 시도 중..."));
			JoinGameSession(SessionSearch->SearchResults[0]);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("참가할 방이 없습니다. 새로운 방을 생성합니다..."));
			CreateSession();
		}

		OnMatchmakingCompleted.Broadcast(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("검색 실패!!! 네트워크 연결을 확인해주세요."));
		OnMatchmakingCompleted.Broadcast(false);
	}
}

void UWGameInstance::JoinGameSession(const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &ThisClass::OnJoinSessionComplete);

		SessionPtr->JoinSession(0, NAME_GameSession, SearchResult);
	}
}

void UWGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);

		if (Result == EOnJoinSessionCompleteResult::Success)
		{
			UE_LOG(LogTemp, Warning, TEXT("세션 참가 성공!!!"));

			FString ConnectInfo;
			if (SessionPtr->GetResolvedConnectString(SessionName, ConnectInfo))
			{
				UE_LOG(LogTemp, Warning, TEXT("서버에 접속 중... : %s"), *ConnectInfo);
				APlayerController* PC = GetFirstLocalPlayerController();
				if (PC)
				{
					PC->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("참가 실패."));
			}
		}
	}
}

void UWGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr();
	if (SessionPtr)
	{		
		ServerSessionName = UWNetStatics::GetSessionNameStr();
		if (ServerSessionName.IsEmpty()) 
		{
			ServerSessionName = TEXT("MyLocalSession"); // 기본 방 이름
		}
		FString SessionSearchId = UWNetStatics::GetSessionSearchIdStr();
		if (SessionSearchId.IsEmpty())
		{
			SessionSearchId = TEXT("MyGameRoom"); // 기본 검색 ID
		}
		SessionServerPort = UWNetStatics::GetSessionPort();

		FOnlineSessionSettings OnlineSessionSettings = UWNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, SessionServerPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &ThisClass::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Creating Failed Right Away!!!"));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
		
		UE_LOG(LogTemp, Log, TEXT("#### Create Session With Name: %s, ID: %s, Port: %d"), *ServerSessionName, *SessionSearchId, SessionServerPort);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find session ptr, Terminate Session Server"));
		TerminateSessionServer();
	}
}

void UWGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchId = FGuid::NewGuid();

	FString CoordinatorURL = UWNetStatics::GetCoordinatorURL();

	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("Sending Request Session Creation to URL: %s"), *URL);

	Request->SetURL(URL);
	Request->SetVerb("POST");

	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(UWNetStatics::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UWNetStatics::GetSessionSearchIdKey().ToString(), SessionSearchId.ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SessionCreationRequestCompleted, SessionSearchId);

	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Request ProcessRequest Failed"));
	}
}

void UWGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully, FGuid SessionSearchId)
{
	if (bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection responded with connection was not successful!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Connection to Coordinator Successfully!"));
}

void UWGameInstance::OnSessionCreated(FName SessionName, bool bSuccess)
{
	if (IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr())
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
	
	 if (bSuccess)
	 {
		UE_LOG(LogTemp, Warning, TEXT("세션 생성 성공!!!"));
	 	GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &ThisClass::WiatPlayerJoinTimeOutReached, WaitPlayerJoinTimeOutDuration);
	 	LoadLevelAndListen(LobbyLevel);
	 }
	 else
	 {
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Failed"));
		 TerminateSessionServer();
	 }
}

void UWGameInstance::EndSessionComplete(FName SessionName, bool bSuccess)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UWGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UWNetStatics::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &ThisClass::EndSessionComplete);
		if (!SessionPtr->EndSession(FName(ServerSessionName)))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UWGameInstance::WiatPlayerJoinTimeOutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Server shut down after %f seconds without player joining"), WaitPlayerJoinTimeOutDuration);
	TerminateSessionServer();
}

void UWGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));

	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Traveling to : %s"), *(TravelStr));
		GetWorld()->ServerTravel(TravelStr);
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