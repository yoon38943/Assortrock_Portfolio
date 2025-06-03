#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SelectCharacterPlayerController.generated.h"

UCLASS()
class WILLBEAOS_API ASelectCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SelectCharacterWidgetClass;
};
