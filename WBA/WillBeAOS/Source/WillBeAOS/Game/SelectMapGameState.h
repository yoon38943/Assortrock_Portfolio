#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "SelectCharacterPlayerController.h"
#include "GameFramework/GameState.h"
#include "SelectMapGameState.generated.h"


UCLASS()
class WILLBEAOS_API ASelectMapGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// TMap은 Replicated가 안되기에 TArray로 교체
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_UpdateWidget)
	TArray<FPlayerInfoStruct> BlueTeamPlayerInfo;
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_UpdateWidget)
	TArray<FPlayerInfoStruct> RedTeamPlayerInfo;
	
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
	TArray<ASelectCharacterPlayerController*> ReadyPlayers;
	
	void CheckPlayerIsReady(ASelectCharacterPlayerController* PC);

	void AddSelectCharacterToPlayerInfo(const FString& PlayerName, TSubclassOf<APawn>& ChosenChar, E_TeamID& Team);

	// 모든 플레이어가 캐릭터를 선택 했는지 체크
	void AllPlayerChosenChar();

	// 캐릭터 선택 공백시 모든 플레이어 로비 복귀
	void AllPlayerBackToLobby();

	// 캐릭터 선택 후 정보 인스턴스에 업로드
	void UploadStateToGameInstance();

	UFUNCTION()
	void OnRep_UpdateWidget();
};
