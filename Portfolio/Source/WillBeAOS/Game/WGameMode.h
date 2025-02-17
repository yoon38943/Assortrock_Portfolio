#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Destructible.h"
#include "WGameMode.generated.h"

UCLASS(config = Game)
class WILLBEAOS_API AWGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AWGameMode();
	
	UPROPERTY(BlueprintReadWrite)
	TArray<class APlayerController*> AllPlayerController;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AWGameState* WGS;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TSoftObjectPtr<UWorld> L_MainMenu;

public:	
	void SpawnTower();
	void PlayerAssignTeam();
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
	UFUNCTION(Server, Reliable)
	void RespawnPlayer(APawn* Player, AController* PlayerController);

	// 몬스터 사망시 이벤트
	UFUNCTION()
	void OnObjectKilled(TScriptInterface<IDestructible> DestroyedObject, AController* Killer);
};
