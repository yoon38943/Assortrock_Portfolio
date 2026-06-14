#include "Game/ServerSessionPlayerController.h"
#include "LoadingScreenWidget.h"


void AServerSessionPlayerController::ClientShowLoadingScreen_Implementation(int32 MaxPlayers)
{
	MatchingWidget = CreateWidget<ULoadingScreenWidget>(this, MatchingWidgetClass);
	if (MatchingWidget)
	{
		MatchingWidget->AddToViewport(0);
		MatchingWidget->UpdateSessionMaxPlayers(MaxPlayers);
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

void AServerSessionPlayerController::CloseDefaultLoadingScreen_Implementation()
{
	// 블루프린트에서 StopLoadingScreen() 실행
}
