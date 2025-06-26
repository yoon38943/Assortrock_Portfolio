#pragma once

#include <rapidjson/internal/meta.h>

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "GameFramework/GameState.h"
#include "PlayGameState.generated.h"

class AGamePlayerController;
struct FPlayerInfoStruct;

UCLASS()
class WILLBEAOS_API APlayGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnRep_ChangeGamePhase)
	EGamePhase CurrentGamePhase = EGamePhase::StartGameWaiting;

	virtual void SetGamePhase(EGamePhase NewGamePhase);

	UFUNCTION()
	void OnRep_ChangeGamePhase();

	void EnterCharacterSelectPhase();

	void Client_EnterCharacterSelectPhase();

	void EnterLoadingPhase();

	void EnterInGamePhase();

	void Client_EnterInGamePhase();


	
	// ---------------------------------------------
	// 플레이 캐릭터 선택 게임 스테이트
	// ---------------------------------------------
public:
	virtual void BeginPlay() override;
	
	// TMap은 Replicated가 안되기에 TArray로 교체
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_UpdateWidget)
	TArray<FPlayerInfoStruct> BlueTeamPlayerInfo;
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_UpdateWidget)
	TArray<FPlayerInfoStruct> RedTeamPlayerInfo;

	UFUNCTION()
	void OnRep_UpdateWidget();
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 BlueTeamPlayersNum;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 RedTeamPlayersNum;

	// 캐릭터 선택 카운트 다운
	UPROPERTY(BlueprintReadOnly, Replicated)
	float SelectCountdown = 20.0;
	FTimerHandle CountdownTimerHandle;

	void UpdateCountdown();

	// 플레이어 맵 로드 완료
	TArray<AGamePlayerController*> ReadyPlayers;
	
	void CheckPlayerIsReady(AGamePlayerController* PC);

	void AddSelectCharacterToPlayerInfo(const FString& PlayerName, TSubclassOf<APawn>& ChosenChar, E_TeamID& Team);

	// 모든 플레이어가 캐릭터를 선택 했는지 체크
	void AllPlayerChosenChar();

	// 캐릭터 선택 공백시 모든 플레이어 로비 복귀
	void AllPlayerBackToLobby();

	// 캐릭터 선택 후 정보 인스턴스에 업로드
	void UploadStateToGameInstance();
};
