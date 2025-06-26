#include "PersistentGame/PlayGameState.h"

#include "GamePlayerController.h"
#include "GamePlayerState.h"
#include "PlayGameMode.h"
#include "Game/WGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


void APlayGameState::SetGamePhase(EGamePhase NewGamePhase)
{
	if (HasAuthority() && CurrentGamePhase != NewGamePhase)
	{
		CurrentGamePhase = NewGamePhase;

		switch (CurrentGamePhase)
		{
		case EGamePhase::CharacterSelect:
			EnterCharacterSelectPhase();
			break;
		case EGamePhase::LoadingPhase:
			EnterLoadingPhase();
			break;
		}
	}
}

void APlayGameState::OnRep_ChangeGamePhase()
{
	if (!HasAuthority())
	{
		switch (CurrentGamePhase)
		{
		case EGamePhase::CharacterSelect:
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::Client_EnterCharacterSelectPhase, 0.05f, false);
				break;
			}
		case EGamePhase::InGame:
			Client_EnterInGamePhase();
			break;
		}
	}
}

void APlayGameState::EnterCharacterSelectPhase()
{
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

void APlayGameState::Client_EnterCharacterSelectPhase()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC)
	{
		PC->StartCharacterSelectPhase();

		AGamePlayerState* PS = PC->GetPlayerState<AGamePlayerState>();
		if (PS)
		{
			PS->StartCharacterSelectPhase();
		}
	}
}

void APlayGameState::EnterLoadingPhase()
{
	// 맵 전환 로딩창 띄우기
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGamePlayerController* PC = Cast<AGamePlayerController>(Iterator->Get());
		if (PC)
		{
			PC->ToInGameLoading();
		}
	}
}

void APlayGameState::EnterInGamePhase()
{
	// 타워 소환 등등
}

void APlayGameState::Client_EnterInGamePhase()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->StartCharacterSelectPhase();

		AGamePlayerState* PS = PC->GetPlayerState<AGamePlayerState>();
		if (PS)
		{
			PS->StartCharacterSelectPhase();
		}
	}
}

void APlayGameState::BeginPlay()
{
	Super::BeginPlay();
}

void APlayGameState::OnRep_UpdateWidget()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC && PC->SelectCharacterWidget)
	{
		PC->UpdatePlayerWidget();
	}
}

void APlayGameState::UpdateCountdown()
{
	SelectCountdown--;

	if (SelectCountdown <= 0 && CountdownTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

		FTimerHandle CheckTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(CheckTimerHandle, this, &ThisClass::AllPlayerChosenChar, 1.f, false);
	}
}

void APlayGameState::CheckPlayerIsReady(AGamePlayerController* PC)
{
	ReadyPlayers.Add(PC);

	if (ReadyPlayers.Num() == PlayerArray.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("모든 플레이어 맵 로드 완료(SelectChar Map)"));
		GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ThisClass::UpdateCountdown, 1.f, true);
	}
}

void APlayGameState::AddSelectCharacterToPlayerInfo(const FString& PlayerName, TSubclassOf<APawn>& ChosenChar, E_TeamID& Team)
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

void APlayGameState::AllPlayerChosenChar()
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

void APlayGameState::AllPlayerBackToLobby()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AGamePlayerController* PC = Cast<AGamePlayerController>(Iterator->Get());
		if (PC)
		{
			PC->BackToLobby();
		}
	}
}

void APlayGameState::UploadStateToGameInstance()
{
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

	APlayGameMode* GM = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->StartLoading();
	}
}

void APlayGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APlayGameState, BlueTeamPlayerInfo);
	DOREPLIFETIME(APlayGameState, RedTeamPlayerInfo);
	DOREPLIFETIME(APlayGameState, BlueTeamPlayersNum);
	DOREPLIFETIME(APlayGameState, RedTeamPlayersNum);
	DOREPLIFETIME(APlayGameState, SelectCountdown);
	DOREPLIFETIME(APlayGameState, CurrentGamePhase);
}
