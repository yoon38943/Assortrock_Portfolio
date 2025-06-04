#pragma once

#include "CoreMinimal.h"
#include "SelectPlayerController.h"
#include "../WStructure.h"
#include "GameFramework/GameMode.h"
#include "SelectGameMode.generated.h"


UCLASS()
class WILLBEAOS_API ASelectGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ASelectGameMode();
	
	UPROPERTY(BlueprintReadOnly)
	class ASelectGameState* SelectGS;

protected:	//매칭시스템
	TMap<FString, FPlayerValue> SelectPlayerReadyStatus;
	
	void LoadPlayerTeamsFromGameInstance();
public:
	UFUNCTION()
	void SetIsReady(APlayerController* PlayerController, E_TeamID TeamVal,bool bReady, TSubclassOf<APawn> PawnClass);
	// 모든 플레이어가 준비되었는지 확인
	void CheckMatch();
	// 플레이어 수가 충족되었는지 확인
	bool AreAllPlayersReady();

	bool AreTeambalanced();
	
	void SaveMatchPlayersToGameInstance();

	void ReturnToMainMenu();
	

public:	//맵 이동
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TSoftObjectPtr<UWorld> WGameMap;
	//SoftObjectPtr의 맵 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuMap;
	
	UFUNCTION(BlueprintCallable)
	void MapTranslate();
protected:
	FTimerHandle DecreaseTimerHandle;
public:
	UFUNCTION()
	void DecreaseTimer();

	//UI 업데이트
public:
	UFUNCTION(BlueprintCallable)
	void UpdateCharName(int32 MWidgetID, FText MCharName);

	UFUNCTION(BlueprintCallable)
	void SetTeamSlotIsChecked(int32 TeamID, int32 UncheckTeamID, FText UserNickName);

	UFUNCTION(BlueprintCallable)
	void SetUserData(int32 MWidgetID, FText MUserNickName);


protected:
	
	virtual void BeginPlay() override;
};
