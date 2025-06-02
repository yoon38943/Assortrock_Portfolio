#include "Game/SeverSessionPlayerController.h"

#include "LoadingScreenWidget.h"
#include "OnlineSessionSettings.h"
#include "WGameInstance.h"


void ASeverSessionPlayerController::ClientShowLoadingScreen_Implementation()
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

void ASeverSessionPlayerController::Client_UpdatePlayerCount_Implementation(int32 CurrentPlayers)
{
	if (MatchingWidget)
	{
		MatchingWidget->UpdateSessionPlayersNum(CurrentPlayers);
	}
}
