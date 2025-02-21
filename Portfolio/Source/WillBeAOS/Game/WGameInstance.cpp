#include "WGameInstance.h"
#include "GameFramework/PlayerState.h"

void UWGameInstance::SavePlayerTeam(APlayerState* PlayerState, int32 TeamID)
{
	if (PlayerState)
	{
		FString PlayerID = PlayerState->GetPlayerName();  // PlayerName 저장
		SavedPlayerTeams.Add(PlayerID, TeamID);
		UE_LOG(LogTemp, Warning, TEXT("🔹 저장된 플레이어: %s, 팀 ID: %d"), *PlayerID, TeamID);
	}
}

void UWGameInstance::SaveMatchPlayerTeam(FString PlayerName, int32 TeamID, TSubclassOf<class APawn> PawnClass)
{
	if (!PlayerName.IsEmpty())
	{
		MatchPlayerTeams.Add(PlayerName, FPlayerValue(TeamID,false,PawnClass));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerName is Null"));
	}
}

TMap<FString, int32> UWGameInstance::GetSavedTeam()
{
	return SavedPlayerTeams;
}

TMap <FString, FPlayerValue> UWGameInstance::GetMatchTeam()
{
	return MatchPlayerTeams;
}