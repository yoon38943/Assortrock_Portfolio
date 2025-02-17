#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OutPlayerController.generated.h"

class UUserWidget;

UCLASS()
class WILLBEAOS_API AOutPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AOutPlayerController();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MainMenuClass;

	UPROPERTY()
	UUserWidget* MainMenu;
	
protected:
	virtual void BeginPlay() override;
};
