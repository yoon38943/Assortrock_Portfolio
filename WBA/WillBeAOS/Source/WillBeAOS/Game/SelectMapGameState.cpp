#include "Game/SelectMapGameState.h"

#include "SelectCharacterGameMode.h"
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

void ASelectMapGameState::UpdateCountdown()
{
	SelectCountdown--;

	if (SelectCountdown <= 0 && CountdownTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

		FTimerHandle CheckTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ThisClass::AllPlayerChosenChar, 1.f, false);
	}
}

void ASelectMapGameState::CheckPlayerIsReady(ASelectCharacterPlayerController* PC)
{
	ReadyPlayers.Add(PC);

	if (ReadyPlayers.Num() == PlayerArray.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("모든 플레이어 맵 로드 완료(SelectChar Map)"));
		GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ThisClass::UpdateCountdown, 1.f, true);
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

void ASelectMapGameState::AllPlayerChosenChar()
{
	for (auto& BlueTeam : BlueTeamPlayerInfo)
	{
		if (BlueTeam.SelectedCharacter == nullptr)
		{
			AllPlayerBackToLobby();
			return;
		}
	}

	for (auto& RedTeam : RedTeamPlayerInfo)
	{
		if (RedTeam.SelectedCharacter == nullptr)
		{
			AllPlayerBackToLobby();
			return;
		}
	}

	UploadStateToGameInstance();
}

void ASelectMapGameState::AllPlayerBackToLobby()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ASelectCharacterPlayerController* PC = Cast<ASelectCharacterPlayerController>(Iterator->Get());
		if (PC)
		{
			PC->BackToLobby();
		}
	}
}

void ASelectMapGameState::UploadStateToGameInstance()
{
	// 맵 전환 로딩창 띄우기
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ASelectCharacterPlayerController* PC = Cast<ASelectCharacterPlayerController>(Iterator->Get());
		if (PC)
		{
			PC->ToInGameLoading();
		}
	}

	// 인스턴스에 정보 업로드
	UE_LOG(LogTemp, Log, TEXT("인스턴스에 캐릭터 선택 정보 업로드..."));
	
	UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
	if (GI)
	{
		for (auto& Elem : GI->MatchPlayersTeamInfo)
		{
			for (auto& BlueTeam : BlueTeamPlayerInfo)
			{
				if (Elem.Key == BlueTeam.PlayerName)
				{
					Elem.Value.SelectedCharacter = BlueTeam.SelectedCharacter;
				}
			}

			for (auto& RedTeam : RedTeamPlayerInfo)
			{
				if (Elem.Key == RedTeam.PlayerName)
				{
					Elem.Value.SelectedCharacter = RedTeam.SelectedCharacter;
				}
			}
		}
	}

	ASelectCharacterGameMode* GM = Cast<ASelectCharacterGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->StartInGame();
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
	DOREPLIFETIME(ASelectMapGameState, SelectCountdown);
}
