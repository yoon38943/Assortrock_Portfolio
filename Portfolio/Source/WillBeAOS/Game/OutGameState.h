#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OutGameState.generated.h"

UCLASS()
class WILLBEAOS_API AOutGameState : public AGameState
{
	GENERATED_BODY()
	// 현재 접속한 플레이어 수 (클라이언트에 복제됨)
public:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount, BlueprintReadOnly, Category = "Match")
	int32 CurrentPlayerCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_IsMatched, BlueprintReadOnly, Category = "Match")
	bool IsMatched = false;
	// 플레이어 수가 변경될 때 호출될 함수
	UFUNCTION()
	void OnRep_PlayerCount();
	UFUNCTION()
	void OnRep_IsMatched();

	UFUNCTION(NetMulticast, reliable)
	void Controll();
};
