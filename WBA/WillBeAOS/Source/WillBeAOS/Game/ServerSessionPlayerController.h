#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ServerSessionPlayerController.generated.h"

UCLASS()
class WILLBEAOS_API AServerSessionPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	class ULoadingScreenWidget* MatchingWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MatchingWidgetClass;

	UFUNCTION(Client, Reliable)
	void ClientShowLoadingScreen();

	UFUNCTION(Client, Reliable)
	void Client_UpdatePlayerCount(int32 CurrentPlayers);

	UFUNCTION(Client, Reliable)
	void Client_UpdateMatchingState(bool bIsFull);
};
