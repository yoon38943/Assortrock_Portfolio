#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "OutGameState.generated.h"

UCLASS()
class WILLBEAOS_API AOutGameState : public AGameState
{
	GENERATED_BODY()
public:

	//접속한 플레이어 컨트롤러
	UPROPERTY(EditAnywhere)
	TArray<class AOutPlayerController*> PlayerControllers;

	UPROPERTY(ReplicatedUsing = OnRep_IsMatched, BlueprintReadOnly, Category = "Match")
	bool IsMatched = false;
	// 플레이어 수가 변경될 때 호출될 함수
	UFUNCTION()
	void OnRep_PlayerCount();
	UFUNCTION()
	void OnRep_IsMatched();
	
	UFUNCTION(NetMulticast, reliable)
	void ControllerIsReady();

	void BeginPlay();
};
