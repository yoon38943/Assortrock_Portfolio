#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SelectCharacterGameMode.generated.h"

UCLASS()
class WILLBEAOS_API ASelectCharacterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	void StartInGame();
};
