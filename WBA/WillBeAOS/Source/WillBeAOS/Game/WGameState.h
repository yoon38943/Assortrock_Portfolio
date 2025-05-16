#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "WStructure.h"
#include "GameFramework/GameState.h"
#include "WGameState.generated.h"

class AWPlayerState;
class ATower;
class ANexus;
class AAOSActor;

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
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FPlayerValue> MatchedPlayers;
	void GetPlayerNameFromInstance();
	
	//접속한 플레이어 컨트롤러
	UPROPERTY(EditAnywhere)
	TArray<class AWPlayerController*> PlayerControllers;
	
	void CheckPlayerIsReady();
	UFUNCTION(BlueprintCallable)
	bool IsAllPlayerIsReady();

	void SetPlayerState(AWPlayerState* WPlayerState, FPlayerValue WPlayerValue);
	
	UPROPERTY(BlueprintReadWrite, Category = "Players")
	TArray<AWPlayerState*> ConnectedPlayerStates;
	
	void RemovePlayer(AWPlayerController* WPlayerController);
	
public: //PlayerReady단계
	void SpawnPlayer();

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
	UPROPERTY(BlueprintReadWrite)
	int32 RespawnTime = 5;

public://타워 관련
	void AddTowerArray(AAOSActor* SpawnedActor);
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tower")
	TArray<ATower*> BlueTowerArray = {};
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tower")
	TArray<ATower*> RedTowerArray = {};
	
	UFUNCTION(BlueprintCallable, Category = "Tower")
	int32 GetBlueTowerNum();
	UFUNCTION(BlueprintCallable, Category = "Tower")
	int32 GetRedTowerNum();
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Tower")
	void RemoveTower(ATower* WTower);

protected://넥서스
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tower")
	ANexus* BlueNexus;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tower")
	ANexus* RedNexus;

public:
	float GetBlueNexusHP();
	float GetRedNexusHP();
	
protected:
	virtual void BeginPlay();
};