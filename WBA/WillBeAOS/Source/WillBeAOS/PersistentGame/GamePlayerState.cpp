#include "PersistentGame/GamePlayerState.h"

#include "GamePlayerController.h"
#include "PlayGameState.h"
#include "Game/WGameInstance.h"
#include "Net/UnrealNetwork.h"


void AGamePlayerState::StartCharacterSelectPhase()
{
	Server_ReplicatePlayerInfo(GetPlayerName());
}

void AGamePlayerState::OnRep_AddWidget()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetPlayerController());
	if (PC)
	{
		PC->PlayerStateInfoReady();
	}
}

void AGamePlayerState::Server_ReplicatePlayerInfo_Implementation(const FString& ClientPlayerName)
{
	UE_LOG(LogTemp, Log, TEXT("플레이어 네임 : %s"), *ClientPlayerName);
	
	PlayerInfo.PlayerName = *ClientPlayerName;

	UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
	if (GI)
	{
		UE_LOG(LogTemp, Log, TEXT("플레이어 네임 : %s - 맵 이동 완료(Select Character Map)"), *PlayerInfo.PlayerName);
		
		if (GI->MatchPlayersTeamInfo.Contains(PlayerInfo.PlayerName))
		{
			PlayerInfo = GI->GetSavedPlayerTeamInfo()[PlayerInfo.PlayerName];
			UE_LOG(LogTemp, Log, TEXT("플레이어 팀 정보 : %s"), PlayerInfo.PlayerTeam == E_TeamID::Blue? TEXT("블루팀") : TEXT("레드팀"));
		}
	}
}

void AGamePlayerState::Server_ChooseTheCharacter_Implementation(TSubclassOf<APawn> ChosenChar)
{
	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	{
		GS->AddSelectCharacterToPlayerInfo(GetPlayerName(), ChosenChar, PlayerInfo.PlayerTeam);
	}
}

void AGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGamePlayerState, PlayerInfo);
}