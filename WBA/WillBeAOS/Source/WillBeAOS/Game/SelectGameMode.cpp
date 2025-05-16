#include "Game/SelectGameMode.h"
#include "SelectGameState.h"
#include "WEnumFile.h"
#include "WGameInstance.h"
#include "GameFramework/PlayerState.h"


ASelectGameMode::ASelectGameMode()
{
	if (!HasAuthority()) // 클라이언트가 GameMode를 생성하지 못하도록 제한
	{
		Destroy();
	}
}

void ASelectGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("GameMode BeginPlay!"));
	ASelectGameState* SelectGameState = Cast<ASelectGameState>(GetWorld()->GetGameState());
	if (SelectGameState != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("GameMode GameState 참조!"));
		SelectGS = SelectGameState;
	}
	
	GetWorld()->GetTimerManager().SetTimer(DecreaseTimerHandle, this, &ThisClass::DecreaseTimer, 1.0f, true);

	LoadPlayerTeamsFromGameInstance();
}

void ASelectGameMode::LoadPlayerTeamsFromGameInstance()
{
	UWGameInstance* WGI = Cast<UWGameInstance>(GetGameInstance());
	if (WGI)
	{
		TMap<FString, E_TeamID> LoadedMap = WGI->GetSavedTeam();
		for (auto& It : LoadedMap)
		{
			FString PlayerName = It.Key;
			E_TeamID TeamID = It.Value;
			if (!PlayerName.IsEmpty())
			{
				SelectPlayerReadyStatus.Add(PlayerName, FPlayerValue(TeamID, false,nullptr));
				UE_LOG(LogTemp, Log, TEXT("🔹 불러온 PlayerController: %s, TeamID: %d"), *PlayerName, TeamID);
			}
		}
	}
}

void ASelectGameMode::UpdateCharName(int32 MWidgetID, FText MCharName)
{
	UE_LOG(LogTemp, Log, TEXT("GameModeUpdateCharName"));

	SelectGS->M_UpdateCharName(MWidgetID, MCharName);
}

void ASelectGameMode::SetTeamSlotIsChecked(int32 TeamID, int32 UncheckTeamID, FText UserNickName)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("GameModeSetTeamSlotIsChecked"));
	if (UncheckTeamID == 0)
	{
		SelectGS->M_UpdateTeamSlot(TeamID, UserNickName);
	}
	else
	{
		SelectGS->M_UnCheckTeamSlot(UncheckTeamID);
		SelectGS->M_UpdateTeamSlot(TeamID, UserNickName);	
	}
}

void ASelectGameMode::SetUserData(int32 MWidgetID, FText MUserNickName)
{
	UE_LOG(LogTemp, Log, TEXT("GameModeSetUserData!"));
}

void ASelectGameMode::DecreaseTimer()
{
	UE_LOG(LogTemp, Log, TEXT("GameModeDecreaseTimer()"));

	SelectGS->SelectTime--;
}

void ASelectGameMode::SetIsReady(APlayerController* PlayerController,E_TeamID TeamVal, bool bReady, TSubclassOf<APawn> PawnClass)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("SetPlayerReady is called"));
	if (!PlayerController)
	{
		return;
	}
	
	FString PlayerName = PlayerController->PlayerState->GetPlayerName();
	
	FPlayerValue* Value = SelectPlayerReadyStatus.Find(PlayerName);
	
	if (Value)
	{
		Value->TeamValue = TeamVal;
		Value->IsReady = bReady;
		Value->WPawnClass = PawnClass;
		
		UE_LOG(LogTemp, Log, TEXT("SelectPlayerReadyStatus 찾았음!, %d"),bReady);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SelectPlayerReadyStatus 새로 추가!"));
		SelectPlayerReadyStatus.Add(PlayerName, FPlayerValue(TeamVal, bReady, PawnClass));
	}
}

void ASelectGameMode::CheckMatch()//매치 시작
{
	if (AreAllPlayersReady())
	{
		UE_LOG(LogTemp, Log, TEXT("AllPlayersReady"));
		if (AreTeambalanced())
		{
			UE_LOG(LogTemp, Log, TEXT("Matchisbalanced"));
			SaveMatchPlayersToGameInstance();
		}
		else
		{
			ReturnToMainMenu();
		}
	}
	else
	{
		ReturnToMainMenu();
	}
}

bool ASelectGameMode::AreAllPlayersReady()
{
	for (auto It = SelectPlayerReadyStatus.CreateIterator(); It; ++It)
	{
		if (It->Key.IsEmpty()) // 플레이어가 떠났다면 제거
		{
			UE_LOG(LogTemp, Log, TEXT("Key 없음!, %s"),*It->Key);
			return false;
		}

		if (!It->Value.IsReady) // 아직 준비되지 않은 플레이어가 있으면 리턴
		{
			UE_LOG(LogTemp, Log, TEXT("준비가 되지않았음!, %s %d"),*It->Key,It->Value.IsReady);
			return false;
		}
	}
	return true; // 모든 플레이어가 준비 완료됨
}

bool ASelectGameMode::AreTeambalanced()
{
	int32 BlueTeamCount= 0;
	int32 RedTeamCount= 0;
	
	for (auto It = SelectPlayerReadyStatus.CreateIterator(); It; ++It)
	{
		if (It->Value.TeamValue==E_TeamID::Blue)
		{
			BlueTeamCount++;
		}
		else if (It->Value.TeamValue==E_TeamID::Red)
		{
			RedTeamCount++;
		}
	}
	
	if (RedTeamCount != 0 && BlueTeamCount != 0 && BlueTeamCount == RedTeamCount)
	{
		return true;
	}
	else return false;
}

void ASelectGameMode::SaveMatchPlayersToGameInstance()
{
	UWGameInstance* WGI = Cast<UWGameInstance>(GetGameInstance());
	if (WGI)
	{
		for (auto& It : SelectPlayerReadyStatus)
		{
			FString PlayerName = It.Key;
			if (!PlayerName.IsEmpty())
			{
				UE_LOG(LogTemp, Log, TEXT("SaveMatchPlayerTeam!"));
				WGI->SaveMatchPlayerTeam(It.Key, It.Value.TeamValue, It.Value.WPawnClass );
			}
		}
	}
	
	MapTranslate();
}

void ASelectGameMode::ReturnToMainMenu()
{
	if (MainMenuMap.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuMap is not set in GameMode!"));
		return;
	}
		
	FString MainMap = MainMenuMap.GetAssetName();
		
	GetWorld()->ServerTravel(MainMap);
}

void ASelectGameMode::MapTranslate()
{
	if (WGameMap.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectMap is not set in GameMode!"));
		return;
	}
	
	FString NextMap = WGameMap.GetAssetName();
	
	GetWorld()->ServerTravel(NextMap);
}
