#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "WEnumFile.h"
#include "WStructure.h"
#include "GameFramework/GameState.h"
#include "WGameState.generated.h"

class AWPlayerController;
class AWPlayerState;
class ATower;
class ANexus;
class AAOSActor;

DECLARE_DELEGATE_TwoParams(FDelegateShowKillState, int32, int32);

UCLASS()
class WILLBEAOS_API AWGameState : public AGameState
{
	GENERATED_BODY()
	
public://GameInit 단계
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	class AWGameMode* WGameMode;
	
	UPROPERTY(BlueprintReadWrite, Category = "State")
	E_GamePlay CurrentGameState = E_GamePlay::Nothing;//게임진행상황을 담은 변수

	int32 GetGMTime = 0;
	FTimerHandle GetGMHandle;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void UpdateGMTimer();
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetGamePlay(E_GamePlay NewState);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void GamePlayStateChanged(E_GamePlay NewState);

	//인스턴스에서 받아온 MatchedPlayers
	TArray<FPlayerInfoStruct> MatchPlayersInfo;
	// Info 클라에게 넘겨주기
	void TakeMatchPlayersInfoToInstance();
	
	//접속한 플레이어 컨트롤러
	UPROPERTY(EditAnywhere)
	TArray<AWPlayerController*> PlayerControllers;
	
	void CheckPlayerIsReady();
	UFUNCTION(BlueprintCallable)
	bool IsAllPlayerIsReady();
	
	UPROPERTY(BlueprintReadWrite, Category = "Players")
	TArray<AWPlayerState*> ConnectedPlayerStates;
	
	void RemovePlayer(AWPlayerController* WPlayerController);
	
public: //PlayerReady단계
	void CheckPlayerSpawned(AWPlayerController* WPlayerController);

	int32 CheckSpawnedPlayers = 0;

	// 모든 플레이어가 준비되었는지 확인하는 함수
	UFUNCTION()
	void CheckAllPlayersReady(); 

public:
	int32 CountdownTime = 0;
protected:
	void ServerCountdown();
public:
	void SetCountdownTime(int32 NewCount);
	
public:
	void SetGameStart();
public://리스폰 관련
	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 RespawnTime = 8;

	FTimerHandle RespawnTimeHandle;
	void AddRespawnTime();

public://타워 관련
	void AssignNexus(AAOSActor* SpawnedActor);

protected://넥서스
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tower")
	ANexus* BlueNexus;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tower")
	ANexus* RedNexus;

public:
	float GetBlueNexusHP();
	float GetRedNexusHP();

	// 캐릭터가 거리 계산할 인스턴스 모음
	TSet<AActor*> ManagedActors;

	// 팀별 킬 점수
	FDelegateShowKillState DelegateShowKillState;
	int32 BlueTeamTotalKillPoints;
	int32 RedTeamTotalKillPoints;

	UFUNCTION(NetMulticast, reliable)
	void NM_ReplicateTotalKillPoints(int32 Blue, int32 Red);

	void CheckKilledTeam(E_TeamID KillTeam);
	
protected:
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};