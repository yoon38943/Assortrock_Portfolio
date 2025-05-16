#pragma once

#include "CoreMinimal.h"
#include "OutPlayerController.h"
#include "GameFramework/GameMode.h"
#include "OutGameMode.generated.h"

UCLASS()
class WILLBEAOS_API AOutGameMode : public AGameMode
{
	GENERATED_BODY()
protected:
	AOutGameMode();
	// 매칭 시스템을 위한 변수
	TMap<TWeakObjectPtr<APlayerController>, bool> PlayerReadyStatus;

	
public:
	// 플레이어의 준비 상태 업데이트
	void SetPlayerReady(APlayerController* Player, bool bReady);
	
	bool IsPlayerAlreadyReady(AOutPlayerController* OutPlayerController);
	
	FTimerHandle MatchStartTimerHandle;

protected:
	
	//SoftObjectPtr의 맵 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
    TSoftObjectPtr<UWorld> SelectMap;
	// 모든 플레이어가 준비되었는지 확인하고 맵 변경
	void CheckMatchAndStartGame();

	void SavePlayerTeamsToGameInstance();
	
	// 맵을 변경하는 함수
	void ChangeToNextMap();

public:
	// 최소 플레이어 수 (블루프린트에서 설정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match")
	int32 MinPlayersToStart = 2;
	
	void UpdateMatchIsReady(bool Matched);
	
	// 플레이어가 접속하면 호출됨
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어가 나가면 호출됨
	virtual void Logout(AController* Exiting) override;

	// 플레이어 수가 충족되었는지 확인
	UFUNCTION(BlueprintCallable)
	bool AreAllPlayersReady();
};
