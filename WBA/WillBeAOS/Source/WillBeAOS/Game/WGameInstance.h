#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "Engine/GameInstance.h"
#include "../WStructure.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "WGameInstance.generated.h"

enum class E_TeamID : uint8;


UCLASS()
class WILLBEAOS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

public:
	UPROPERTY()
	TMap<FString, FPlayerInfoStruct> MatchPlayersTeamInfo;  // PlayerName을 기반으로 팀 정보 저장

	UPROPERTY(BlueprintReadOnly)
	int32 FinalBlueTeamPlayersNum;
	UPROPERTY(BlueprintReadOnly)
	int32 FinalRedTeamPlayersNum;

	void LogFinalTeamNum();

	UPROPERTY()
	TMap<FString,FPlayerValue> MatchPlayerTeams;
	
public:
	// GameInstance에 Key 값 저장
	void SavePlayerTeamInfo(FString& PlayerNameInfo, FPlayerInfoStruct PlayerInfo);

	void DeletePlayerTeamInfo(FString PlayerName);

	void SortPlayerTeamInfo(FString ExitingPlayerName);
	
	void SaveMatchPlayerTeam(FString PlayerNameInfo, E_TeamID, TSubclassOf<class APawn> PawnClass);

	TMap<FString, FPlayerInfoStruct> GetSavedPlayerTeamInfo();
	
	TMap <FString, FPlayerValue> GetMatchTeam();

public:
	IOnlineSessionPtr OnlineSessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	class IOnlineSubsystem* Subsystem;

	bool IsSessionFull();

protected:
	// 매칭 관련 함수
	void FindSessions();
	void OnFindSessionComplete(bool Success);
	UFUNCTION(BlueprintCallable)
	void StartJoinSession();
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);	
	void CreateGameSession();
	void OnCreateSessionComplete(FName SessionName, bool Success);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void LeaveGameSession();
	void DestroySession();
	void OnDestroySessionComplete(FName SessionName, bool Success);
	
	const ULocalPlayer* LocalPlayer;

	bool bSessionLeaveBeforeCreateSession = false;
};