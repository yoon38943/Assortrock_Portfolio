#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Destructible.h"
#include "WGameMode.generated.h"

class AWPlayerState;
DECLARE_MULTICAST_DELEGATE(FOnGameEnd);

UCLASS(config = Game)
class WILLBEAOS_API AWGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AWGameMode();

	FOnGameEnd OnGameEnd;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AWGameState* WGS;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> L_MainMenu;
	
public:	
	void SpawnTower();
	
	UPROPERTY(EditAnywhere)
	TArray<class AWPlayerController*> PlayerControllers;
	
	void SetGSPlayerControllers();

	TArray<class APlayerSpawner*> PlayerSpawners;
	void GetPlayerSpawners();
	void SetPlayerSpawners(AWPlayerState* PlayerState);

protected:
	int32 CountdownTime = 0;
	FTimerHandle CountdownHandle;
public:
	void StartCountdown(int32 InitialTime);
	void UpdateCountdown();

public:
	void DestroyWall();
public:
	FTimerHandle SpawnMinionsTimerHandle;
	void SpawnMinions();
	
private:
	TMap<AActor*, int32> TeamMap;      // 팀정보 맵
public:
	// 팀 할당
	void AssignTeam(AActor* Actor, int32 TeamID);
	
	// 팀 검색
	int32 GetTeam(AActor* Actor) const;

	
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual bool ReadyToEndMatch_Implementation() override;
	virtual void Logout(AController* Exiting) override;
	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;


public:
	// 스폰 함수
	UFUNCTION()
	void RespawnPlayer(APawn* Player, AController* PlayerController);

	// 몬스터 사망시 이벤트
	UFUNCTION()
	void OnObjectKilled(TScriptInterface<IDestructible> DestroyedObject, AController* Killer);

	// 넥서스 파괴
	// ---------- 3.4 현규 수정
	void OnNexusDestroyed(E_TeamID LoseTeam);
};
