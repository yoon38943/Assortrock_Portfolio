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
	FTimerHandle MatchStartTimerHandle;

protected:
	
	//SoftObjectPtr의 맵 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
    TSoftObjectPtr<UWorld> SelectMap;
	
	// 맵을 변경하는 함수
	void ChangeToNextMap();

public:	
	// 플레이어가 접속하면 호출됨
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어가 나가면 호출됨
	virtual void Logout(AController* Exiting) override;
};
