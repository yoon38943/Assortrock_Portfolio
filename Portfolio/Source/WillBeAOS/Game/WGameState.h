#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "WGameState.generated.h"

class AWPlayerState;
class ATower;
class ANexus;

UENUM(BlueprintType)
enum class E_GamePlay : uint8
{
	GameInit,
	ReadyCountdown,
	Gameplaying,
	GameEnded
};

UCLASS()
class WILLBEAOS_API AWGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, Category = "State")
	E_GamePlay CurrentGameState;
	
	virtual void BeginPlay();

	float GetNexusHP();
	
	void GetTower();
	
	UFUNCTION(BlueprintCallable, Category = "Tower")
	int32 GetTowerNum();

	UPROPERTY(BlueprintReadOnly, Category = "Tower")
	TArray<ATower*> TowerArray = {};//Ÿ�� ��ü��

	void HandleNexusDestroyed();
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Players")
	TArray<AWPlayerState*> ConnectedPlayerStates;
	
	void AddPlayer(AWPlayerState* PlayerState);
	
	void RemovePlayer(AWPlayerState* PlayerState);
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void StartCountdown(int32 InitialTime);
	
protected:
	int32 CountdownTime = 0;
	
	FTimerHandle CountdownHandle;
	
	void UpdateCountdown();

public:
	//리스폰시 필요한
	UPROPERTY(BlueprintReadWrite)
	int32 RespawnTime = 5;
	
protected:
	ANexus* Nexus;
	
	ANexus* GetNexus();
	
};
