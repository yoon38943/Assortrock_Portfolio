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
	void StartLoading();
	
};
