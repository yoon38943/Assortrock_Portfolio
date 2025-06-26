#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayGameMode.generated.h"

class APlayGameState;

UCLASS()
class WILLBEAOS_API APlayGameMode : public AGameMode
{
	GENERATED_BODY()

	APlayGameState* GS;

	virtual void BeginPlay() override;


	
	// ---------------------------------------------
	// 플레이어 선택 게임 모드
	// ---------------------------------------------
public:
	UPROPERTY(EditAnywhere, Category="Level Streaming")
	TSoftObjectPtr<UWorld> CharacterSelectStream;

	UFUNCTION()
	void SelectLevelOnLoaded();
	
	void StartLoading();


	
	// ---------------------------------------------
	// 인게임 게임 모드
	// ---------------------------------------------
public:
	UPROPERTY(EditAnywhere, Category = "Level Streaming")
	TArray<TSoftObjectPtr<UWorld>> StreamingLevelSequence;

	int32 CurrentStreamingLevelIndex;

	void StartSequentialLevelStreaming();
	
	UFUNCTION()
	void OnSequenceLevelLoaded();
};
