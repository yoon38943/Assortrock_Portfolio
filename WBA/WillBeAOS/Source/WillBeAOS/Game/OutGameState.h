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

	void BeginPlay();
};
