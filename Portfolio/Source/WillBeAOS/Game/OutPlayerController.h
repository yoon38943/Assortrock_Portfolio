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

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MainMenu;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IsMatched();
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ServerSetReady(bool bReady);	
};
