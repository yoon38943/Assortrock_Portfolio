#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerState.generated.h"

UCLASS()
class WILLBEAOS_API AGamePlayerState : public APlayerState
{
	GENERATED_BODY()

	// ---------------------------------------------
	// 플레이 캐릭터 선택 게임 스테이트
	// ---------------------------------------------
public:
	void StartCharacterSelectPhase();
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AddWidget)
	FPlayerInfoStruct PlayerInfo;

	UFUNCTION()
	void OnRep_AddWidget();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_ChooseTheCharacter(TSubclassOf<APawn> ChosenChar);

	UFUNCTION(Server, Reliable)
	void Server_ReplicatePlayerInfo(const FString& ClientPlayerName);
};
