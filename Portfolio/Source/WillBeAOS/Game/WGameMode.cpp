#include "WGameMode.h"
#include "WGameState.h"
#include "Character/WCharacterBase.h"
#include "Character/WPlayerController.h"
#include "Character/WPlayerState.h"
#include "Gimmick/Nexus.h"
#include "Gimmick/Tower.h"
#include "Gimmick/SpawnTowerPoint.h"
#include "Kismet/GameplayStatics.h"

AWGameMode::AWGameMode()
{
	PlayerStateClass = AWPlayerState::StaticClass();
}

void AWGameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);

	if (NewPC)
	{
		AllPlayerController.Add(NewPC);
	}
}



void AWGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (NewPlayer)
	{
		AllPlayerController.Add(Cast<AWPlayerController>(NewPlayer));
	}

	if (GetGameState<AWGameState>() != nullptr)
	{
		WGS = GetGameState<AWGameState>();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("GameState is called"));

		if (AWPlayerState* NewPlayerState = Cast<AWPlayerState>(NewPlayer->PlayerState))
		{
			WGS->AddPlayer(NewPlayerState);
		}
	}
}

void AWGameMode::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Mode BeginPlay called"));
	
	SpawnTower();

	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (CurrentLevel == "L_Portfolio")
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetShowMouseCursor(false);
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
}

bool AWGameMode::ReadyToStartMatch_Implementation()
{
	Super::ReadyToStartMatch_Implementation();
	return true;
}

void AWGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

bool AWGameMode::ReadyToEndMatch_Implementation()
{
	Super::ReadyToEndMatch_Implementation();
	return (WGS != nullptr) && (WGS->CurrentGameState == E_GamePlay::GameEnded);
}

void AWGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AWGameMode::Logout(AController* Exiting)
{
	if (APlayerController* PC = Cast<APlayerController>(Exiting))
	{
		AllPlayerController.Remove(PC);
	}
	Super::Logout(Exiting);
}

//-----------------------------------------------------------
//스폰 타워
//-----------------------------------------------------------


void AWGameMode::SpawnTower()
{
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnTowerPoint::StaticClass(), SpawnPoints);

	for (AActor* SpawnPointActor : SpawnPoints)
	{
		ASpawnTowerPoint* SpawnPoint = Cast<ASpawnTowerPoint>(SpawnPointActor);
		
		if (HasAuthority() && SpawnPoint && SpawnPoint->TowerClass)
		{
			// 타워 생성
			FActorSpawnParameters SpawnParams;
			AActor* Tower = GetWorld()->SpawnActor<AActor>(
				SpawnPoint->TowerClass,
				SpawnPoint->GetActorLocation(),
				SpawnPoint->GetActorRotation(),
				SpawnParams
			);
			
			if (ATower* SpawnedTower = Cast<ATower>(Tower))
			{
				SpawnedTower->SetReplicates(true);
				SpawnedTower->TowerTeamID = SpawnPoint->TeamID;
				AssignTeam(SpawnedTower,static_cast<int32>(SpawnedTower->TowerTeamID));
			}
			else if (ANexus* SpawnedNexus = Cast<ANexus>(Tower))
			{	
				SpawnedNexus->SetReplicates(true);
				SpawnedNexus->NexusTeamID = SpawnPoint->TeamID;
				AssignTeam(SpawnedNexus,static_cast<int32>(SpawnedNexus->NexusTeamID));
			}
		}
	}
}

//-----------------------------------------------------------
//팀 관련 함수
//-----------------------------------------------------------

void AWGameMode::AssignTeam(AActor* Actor, int32 TeamID)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid actor!"));
		return;
	}
	TeamMap.Add(Actor, TeamID);
	UE_LOG(LogTemp, Log, TEXT("Actor Add! %s %d"), *Actor->GetName(), TeamID);
	//	WGS->UpdateTeamInfo(Actor, Team	ID);
}

void AWGameMode::PlayerAssignTeam()
{
	E_TeamID Team = E_TeamID::Blue;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It;++It)
	{
		if (AWPlayerController* PC = Cast<AWPlayerController>(It->Get()))
		{
			if (AWPlayerState* PS = Cast<AWPlayerState>(PC->PlayerState))
			{
				PS->SetTeamID(Team); // 플레이어 상태에 팀 지정
				AssignTeam(PS,static_cast<int32>(PS->TeamID));
				UE_LOG(LogTemp, Log, TEXT("Actor Add! %s %d"), *PS->GetName(), PS->TeamID);
				Team = (Team == E_TeamID::Blue) ? E_TeamID::Red : E_TeamID::Blue; // 번갈아 가며 팀 배정
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerState not found or cast failed."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerController not found or cast failed."));
		}
	}
}

int32 AWGameMode::GetTeam(AActor* Actor) const
{
	if (!Actor)
	{
		return -1; // Invalid actor
	}
	
	if (const int32* TeamID = TeamMap.Find(Actor))
	{
		return *TeamID;
	}

	return -1; // Team 정보 없음
}

// 리스폰
void AWGameMode::RespawnPlayer_Implementation(APawn* Player, AController* PlayerController)
{
	if (!Player || !PlayerController)
	{
		return;
	}
	
	AActor* PlayerStart = FindPlayerStart(PlayerController);
	if (PlayerStart)
	{
		AWCharacterBase* RespawnChar = GetWorld()->SpawnActor<AWCharacterBase>(Player->GetClass(), PlayerStart->GetActorLocation(), FRotator(0,0,0));
		AWPlayerController* PC = Cast<AWPlayerController>(PlayerController);
		
		if (PC)
		{
			PC->OnPossess(RespawnChar);

			AWPlayerState* PS = PC->GetPlayerState<AWPlayerState>();
			if (PS)
			{
				PS->SetHP(PS->GetMaxHP());
			}
		}
	}
}

void AWGameMode::OnObjectKilled(TScriptInterface<IDestructible> DestroyedObject, AController* Killer)
{
	if (!DestroyedObject || !Killer) { return; }

	AWPlayerState* PlayerState = Killer->GetPlayerState<AWPlayerState>();
	if (PlayerState)
	{
		PlayerState->Server_AddGold(DestroyedObject->GetGoldReward());
	}
}
