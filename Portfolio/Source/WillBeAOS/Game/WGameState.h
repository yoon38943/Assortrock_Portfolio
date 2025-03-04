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
	
public://타워 관련
	void AddTowerArray(AAOSActor* SpawnedActor);
	
	void GetTower();
	
	UPROPERTY(BlueprintReadOnly, Category = "Tower")
	TArray<ATower*> BlueTowerArray = {};
	UPROPERTY(BlueprintReadOnly, Category = "Tower")
	TArray<ATower*> RedTowerArray = {};
	
	UFUNCTION(BlueprintCallable, Category = "Tower")
	int32 GetBlueTowerNum();
	UFUNCTION(BlueprintCallable, Category = "Tower")
	int32 GetRedTowerNum();
	
	UFUNCTION(BlueprintCallable, Category = "Tower")
	void RemoveTower(ATower* WTower);

protected://넥서스
	ANexus* BlueNexus;
	ANexus* RedNexus;

public:
	float GetBlueNexusHP();
	float GetRedNexusHP();
	
public://카운트 다운
	UFUNCTION(NetMulticast, Reliable)
	void StartCountdown(int32 InitialTime);
	
protected:
	int32 CountdownTime = 0;
	FTimerHandle CountdownHandle;
	void UpdateCountdown();

public://리스폰 관련
	UPROPERTY(BlueprintReadWrite)
	int32 RespawnTime = 5;
	
protected:
	virtual void BeginPlay();
};