#include "OutGameMode.h"
#include "OutGameState.h"
#include "WEnumFile.h"
#include "WGameInstance.h"

AOutGameMode::AOutGameMode()
{
	if (!HasAuthority()) // 클라이언트가 GameMode를 생성하지 못하도록 제한
	{
		Destroy();
	}
	
	bUseSeamlessTravel = true;
}

void AOutGameMode::SetPlayerReady(APlayerController* Player, bool bReady)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("SetPlayerReady is called"));
	if (!Player)
	{
		return;
	}
	
	PlayerReadyStatus.FindOrAdd(Player) = bReady;
	
	if (PlayerReadyStatus.Contains(Player) && PlayerReadyStatus[Player])
	{
		CheckMatchAndStartGame(); // 상태가 변경될 때마다 확인
	}
}

bool AOutGameMode::IsPlayerAlreadyReady(AOutPlayerController* OutPlayerController)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("IsPlayerAlreayReady is called"));
	if (PlayerReadyStatus.Contains(OutPlayerController))
	{
		return false;
	}

	if (bool* IsReadyPtr = PlayerReadyStatus.Find(OutPlayerController))
	{
		return *IsReadyPtr;
	}
	
	return false;
}

void AOutGameMode::PostLogin(APlayerController* NewPlayer)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Postlogin is called"));
	Super::PostLogin(NewPlayer);
	
	PlayerReadyStatus.Add(NewPlayer, false);
	
	AOutGameState* OutGameState = GetGameState<AOutGameState>();
	if (OutGameState)
	{
		OutGameState->PlayerControllers.Add(Cast<AOutPlayerController>(NewPlayer));
	}
}

bool AOutGameMode::AreAllPlayersReady()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("AreAllPlayerReady is called"));
	if (PlayerReadyStatus.Num() < MinPlayersToStart)
	{
		return false; // 최소 인원 미충족
	}

	for (auto It = PlayerReadyStatus.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid()) // 플레이어가 떠났다면 제거
		{
			It.RemoveCurrent();
			continue;
		}

		if (!It->Value) // 아직 준비되지 않은 플레이어가 있으면 리턴
		{
			return false;
		}
	}
	return true; // 모든 플레이어가 준비 완료됨
}

void AOutGameMode::CheckMatchAndStartGame()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("MatchStart is called"));
	// 모든 플레이어가 준비되었으면 맵 변경
	if (AreAllPlayersReady())
	{
		SavePlayerTeamsToGameInstance();

		UpdateMatchIsReady(true);
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("MapChange is called"));
		GetWorldTimerManager().SetTimer(MatchStartTimerHandle, this, &ThisClass::ChangeToNextMap, 5.0, false);
	}
}

void AOutGameMode::SavePlayerTeamsToGameInstance()
{
	UWGameInstance* WGI = Cast<UWGameInstance>(GetGameInstance());
	if (WGI)
	{
		for (auto& It : PlayerReadyStatus)
		{
			APlayerController* PC = It.Key.Get();
			if (PC)
			{
				APlayerState* PS = PC->PlayerState;
				WGI->SavePlayerTeam(PS, E_TeamID::Neutral);
			}
		}
	}
}

void AOutGameMode::ChangeToNextMap()
{
	if (SelectMap.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectMap is not set in GameMode!"));
		return;
	}
	
	FString NextMap = SelectMap.GetAssetName();
	
	//서버가 같은 맵으로 이동시키는 함수
	GetWorld()->ServerTravel(NextMap);
}

void AOutGameMode::UpdateMatchIsReady(bool Matched)
{
	AOutGameState* OutGameState = GetGameState<AOutGameState>();
	if (OutGameState)
	{
		OutGameState->IsMatched = Matched;
	}
}

void AOutGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	if (APlayerController* ExitingPC = Cast<APlayerController>(Exiting))
	{
		PlayerReadyStatus.Remove(ExitingPC);
	}
	UE_LOG(LogTemp, Warning, TEXT("Exiting Controller InValid!"));
}

