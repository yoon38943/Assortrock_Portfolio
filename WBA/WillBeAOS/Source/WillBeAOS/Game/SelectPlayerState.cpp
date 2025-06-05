#include "Game/SelectPlayerState.h"

#include "SelectCharacterPlayerController.h"
#include "SelectMapGameState.h"
#include "WGameInstance.h"
#include "Net/UnrealNetwork.h"


void ASelectPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())return;

	Server_ReplicatePlayerInfo(GetPlayerName());
}

void ASelectPlayerState::Server_ChooseTheCharacter_Implementation(TSubclassOf<APawn> ChosenChar)
{
	ASelectMapGameState* GS = Cast<ASelectMapGameState>(GetWorld()->GetGameState());
	{
		GS->AddSelectCharacterToPlayerInfo(GetPlayerName(), ChosenChar, PlayerInfo.PlayerTeam);
	}
}

void ASelectPlayerState::OnRep_AddWidget()
{
	ASelectCharacterPlayerController* PC = Cast<ASelectCharacterPlayerController>(GetPlayerController());
	if (PC)
	{
		PC->PlayerStateInfoReady();
	}
}

void ASelectPlayerState::Server_ReplicatePlayerInfo_Implementation(const FString& ClientPlayerName)
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

void ASelectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASelectPlayerState, PlayerInfo);
}
