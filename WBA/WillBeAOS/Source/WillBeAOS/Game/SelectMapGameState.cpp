#include "Game/SelectMapGameState.h"

#include "SelectCharacterPlayerController.h"
#include "WGameInstance.h"
#include "Net/UnrealNetwork.h"

void ASelectMapGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
	if (GI)
	{
		for (auto& Elem : GI->MatchPlayersTeamInfo)
		{
			if (Elem.Value.PlayerTeam == E_TeamID::Blue)
			{
				BlueTeamPlayerInfo.Add(Elem.Value);
			}
			else if (Elem.Value.PlayerTeam == E_TeamID::Red)
			{
				RedTeamPlayerInfo.Add(Elem.Value);
			}
		}
		
		BlueTeamPlayersNum = GI->FinalBlueTeamPlayersNum;
		RedTeamPlayersNum = GI->FinalRedTeamPlayersNum;

		UE_LOG(LogTemp, Log, TEXT("게임 인스턴스 -> 게임 스테이트 정보 받아오기 완료!!!(Select Character Map)"));
	}
}

void ASelectMapGameState::AddSelectCharacterToPlayerInfo(const FString& PlayerName, TSubclassOf<APawn>& ChosenChar, E_TeamID& Team)
{
	if (Team == E_TeamID::Blue)
	{
		for (auto& Elem : BlueTeamPlayerInfo)
		{
			if (Elem.PlayerName == PlayerName)
			{
				Elem.SelectedCharacter = ChosenChar;
			}
		}
	}
	else if (Team == E_TeamID::Red)
	{
		for (auto& Elem : RedTeamPlayerInfo)
		{
			if (Elem.PlayerName == PlayerName)
			{
				Elem.SelectedCharacter = ChosenChar;
			}
		}
	}
}

void ASelectMapGameState::OnRep_UpdateWidget()
{
	ASelectCharacterPlayerController* PC = Cast<ASelectCharacterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC && PC->SelectCharacterWidget)
	{
		PC->UpdatePlayerWidget();
	}
}

void ASelectMapGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASelectMapGameState, BlueTeamPlayerInfo);
	DOREPLIFETIME(ASelectMapGameState, RedTeamPlayerInfo);
	DOREPLIFETIME(ASelectMapGameState, BlueTeamPlayersNum);
	DOREPLIFETIME(ASelectMapGameState, RedTeamPlayersNum);
}
