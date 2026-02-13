#include "Game/Network/WGameSession.h"
#include "Game/WGameInstance.h"

bool AWGameSession::ProcessAutoLogin()
{
	return true;
}

void AWGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (UWGameInstance* GameInst = GetGameInstance<UWGameInstance>())
	{
		GameInst->PlayerJoined(UniqueId);
	}
}

void AWGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);
	if (UWGameInstance* GameInst = GetGameInstance<UWGameInstance>())
	{
		GameInst->PlayerLeft(UniqueId);
	}
}
