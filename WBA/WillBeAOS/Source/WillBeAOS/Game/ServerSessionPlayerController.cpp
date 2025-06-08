#include "Game/ServerSessionPlayerController.h"

#include "LoadingScreenWidget.h"
#include "OnlineSessionSettings.h"
#include "WGameInstance.h"


void AServerSessionPlayerController::ClientShowLoadingScreen_Implementation()
{
	MatchingWidget = CreateWidget<ULoadingScreenWidget>(this, MatchingWidgetClass);
	if (MatchingWidget)
	{
		MatchingWidget->AddToViewport(0);
		UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
		if (GI && GI->OnlineSessionInterface.IsValid())
		{
			FNamedOnlineSession* Session = GI->OnlineSessionInterface->GetNamedSession(NAME_GameSession);
			if (Session)
			{
				MatchingWidget->UpdateSessionMaxPlayers(Session->SessionSettings.NumPublicConnections);
			}
		}
	}
}

void AServerSessionPlayerController::Client_UpdatePlayerCount_Implementation(int32 CurrentPlayers)
{
	if (MatchingWidget)
	{
		MatchingWidget->UpdateSessionPlayersNum(CurrentPlayers);
	}
}

void AServerSessionPlayerController::Client_UpdateMatchingState_Implementation(bool bIsFull)
{
	if (MatchingWidget)
	{
		MatchingWidget->bIsMatchFull = bIsFull;
	}
}
