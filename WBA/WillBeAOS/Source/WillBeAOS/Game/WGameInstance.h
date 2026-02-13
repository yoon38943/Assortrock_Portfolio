#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "Engine/GameInstance.h"
#include "Http.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WGameInstance.generated.h"

enum class E_TeamID : uint8;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompleted, bool /*bWasSuccessful*/, const FString& /*UserId*/, const FString& /*Error*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchmakingCompleted, bool /*bWasSuccessful*/);

UCLASS()
class WILLBEAOS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

public:
	UFUNCTION(BlueprintCallable, Category = Matchmaking)
	void StartMatchmaking();

	FOnMatchmakingCompleted OnMatchmakingCompleted;
	
/******************************************/
/*				  로그인                   */
/******************************************/
public:
	bool IsLoggedIn() const;
	bool IsLoggingIn() const;
	void ClientAccountPortalLogin();

	FOnLoginCompleted OnLoginCompleted;
	
private:
	void ClinetLogin(const FString& Type, const FString& Id, const FString& Token);
	void LoginCompleted(int NumOfLocalPlayers, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	FDelegateHandle LoggingInDelegatedHandle;

/******************************************/
/*              세션 관리 부분              */
/******************************************/
public:
	void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId);

	void RequestCreateAndJoinSession(const FName& NewSessionName);

private:
	void SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully, FGuid SessionSearchId);
	
	void FindSessions();
	void OnFindeSessionsComplete(bool bWasSuccessful);

	void JoinGameSession(const FOnlineSessionSearchResult& SearchResult);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	void CreateSession();
	void OnSessionCreated(FName SessionName, bool bSuccess);
	void EndSessionComplete(FName SessionName, bool bSuccess);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FString ServerSessionName;
	int SessionServerPort;

	void TerminateSessionServer();

	FTimerHandle WaitPlayerJoinTimeoutHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Sesssion")
	float WaitPlayerJoinTimeOutDuration = 60.f;

	void WiatPlayerJoinTimeOutReached();

	TSet<FUniqueNetIdRepl> PlayerRecord;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;
	
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);

	// 맵의 경로 지정
	UPROPERTY(EditAnywhere, Category = "Map")
	TSoftObjectPtr<UWorld> OutLobbyMap;

public:
	UPROPERTY()
	TMap<FString, FPlayerInfoStruct> MatchPlayersTeamInfo;  // PlayerName을 기반으로 팀 정보 저장

	UPROPERTY(BlueprintReadOnly)
	int32 FinalBlueTeamPlayersNum;
	UPROPERTY(BlueprintReadOnly)
	int32 FinalRedTeamPlayersNum;

	void LogFinalTeamNum();
	
public:
	// GameInstance에 Key 값 저장
	void SavePlayerTeamInfo(FString& PlayerNameInfo, FPlayerInfoStruct PlayerInfo);

	void DeletePlayerTeamInfo(FString PlayerName);

	void SortPlayerTeamInfo(FString ExitingPlayerName);
	
	TMap<FString, FPlayerInfoStruct> GetSavedPlayerTeamInfo();

	void AssignPlayerNickName();
};