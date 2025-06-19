#include "WGameMode.h"
#include "WGameState.h"
#include "Character/AOSCharacter.h"
#include "Character/Char_Wraith.h"
#include "Character/WCharacterBase.h"
#include "Character/WPlayerController.h"
#include "Character/WPlayerState.h"
#include "Gimmick/PlayerSpawner.h"
#include "Gimmick/Tower.h"
#include "Gimmick/SpawnTowerPoint.h"
#include "Gimmick/StartWall.h"
#include "Kismet/GameplayStatics.h"
#include "Minions/MinionsSpawner.h"

AWGameMode::AWGameMode()
{
	PlayerStateClass = AWPlayerState::StaticClass();
}

void AWGameMode::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Mode BeginPlay called"));
	
	WGS = GetWorld()->GetGameState<AWGameState>();
	if (WGS==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState is nullptr!"));
	}
	
	SpawnTower();
	GetPlayerSpawners();
}

void AWGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp,Log,TEXT("PostLogin %s"),*NewPlayer->GetName());
	
	AWPlayerController* WPlayerController = Cast<AWPlayerController>(NewPlayer);
	if (WPlayerController)
	{
		PlayerControllers.AddUnique(WPlayerController);
		if (WGS)
		{
			SetGSPlayerControllers();
		}
	}
}

void AWGameMode::SetGSPlayerControllers()
{
	WGS->PlayerControllers = PlayerControllers;
}

void AWGameMode::GetPlayerSpawners()
{
	TArray<AActor*> PlayerSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerSpawner::StaticClass(), PlayerSpawnPoints);

	for (auto It:PlayerSpawnPoints)
	{
		if (APlayerSpawner* Spawner = Cast<APlayerSpawner>(It))
		{
			PlayerSpawners.Add(Spawner);
		}
	}
	UE_LOG(LogTemp,Log,TEXT("Player Spawners Num %d "),PlayerSpawners.Num());
}

void AWGameMode::SetPlayerSpawners(class AWPlayerState* PlayerState)
{
	for (auto It: PlayerSpawners)
	{
		if (It->TeamID == PlayerState->PlayerInfo.PlayerTeam)
		{
			PlayerState->PlayerSpawner = It;
		}
	}
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
			
			if (AAOSActor* SpawnedActor = Cast<AAOSActor>(Tower))
			{
				SpawnedActor->SetReplicates(true);
				SpawnedActor->SetTeamID(SpawnPoint->TeamID);
				WGS->AssignNexus(SpawnedActor);
				AssignTeam(SpawnedActor,static_cast<int32>(SpawnedActor->TeamID));
				ATower* TowerColor = Cast<ATower>(SpawnedActor);
				if (TowerColor)
				{
					TowerColor->S_SetHPbarColor();
				}
			}
		}
	}
}

//-----------------------------------------------------------
//팀 관련 함수
//-----------------------------------------------------------

void AWGameMode::StartCountdown(int32 InitialTime)
{
	UE_LOG(LogTemp, Log, TEXT("ServerCountdown"));
	CountdownTime = InitialTime;
	GetWorldTimerManager().SetTimer(CountdownHandle, this, &AWGameMode::UpdateCountdown, 1.f, true);
}

void AWGameMode::UpdateCountdown()
{
	CountdownTime--;
	
	if (CountdownTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownHandle);
		WGS->SetGamePlay(E_GamePlay::Gameplaying);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UpdateCountdown %d"),CountdownTime);
		WGS->SetCountdownTime(CountdownTime);	
	}
}

void AWGameMode::DestroyWall()
{
	TArray<AActor*> Walls;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AStartWall::StaticClass(), Walls);
	for (auto WallActor : Walls)
	{
		if (WallActor->HasAuthority()) {WallActor->Destroy();}
	}
}

void AWGameMode::SpawnMinions()		// 미니언 스폰
{	
	TArray<AActor*> MinionSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AMinionsSpawner::StaticClass(), MinionSpawnPoints);
	if (MinionSpawnPoints.Num() > 0)
	{
	for (auto It:MinionSpawnPoints)
		{
			if (AMinionsSpawner* MinionsSpawner = Cast<AMinionsSpawner>(It))
			{
				MinionsSpawner->StartSpawnMinions();
			}
		}
	}
}

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
void AWGameMode::RespawnPlayer(APawn* Player, AController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}
	
	AWPlayerController* PC = Cast<AWPlayerController>(PlayerController);
	if (PC)
	{
		AWPlayerState* PS = PC->GetPlayerState<AWPlayerState>();
		if (PS)
		{
			if (Player==nullptr)
			{
				SetPlayerSpawners(PS);

				if (!PS->PlayerSpawner)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player Spawner NULL"));
					return;
				}

				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				
				AAOSCharacter* Respawnpawn = GetWorld()->SpawnActor<AAOSCharacter>(PS->PlayerInfo.SelectedCharacter, PS->PlayerSpawner->GetActorLocation(), PS->PlayerSpawner->GetActorRotation(), Params);
				if (Respawnpawn)
				{
					AWCharacterBase* RespawnChar = Cast<AWCharacterBase>(Respawnpawn);
					if (RespawnChar)
					{
						UE_LOG(LogTemp, Log, TEXT("Player Spawner %s, %d"),*PC->GetName(),PS->PlayerInfo.PlayerTeam);

						RespawnChar->CharacterTeam = PS->PlayerInfo.PlayerTeam;

						// 나중에 고칠 부분 --- 캐릭터 많아지면 힘들어짐, 캐릭터 베이스로 퉁칠 수 있을것
						AChar_Wraith* RespawnWraith = Cast<AChar_Wraith>(RespawnChar);
						if (RespawnWraith)
						{
							PC->OnPossess(RespawnWraith);
							PC->OnGameStateChanged(E_GamePlay::ReadyCountdown);
							WGS->CheckPlayerSpawned(PC);
							UE_LOG(LogTemp, Log, TEXT("첫 스폰!"));
						}
						else
						{
							PC->OnPossess(RespawnChar);
							WGS->CheckPlayerSpawned(PC);
							UE_LOG(LogTemp, Log, TEXT("첫 스폰!"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("RespawnChar is null!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("PawnClass is null!"));
				}
			}
			else
			{
				AWCharacterBase* RespawnChar = GetWorld()->SpawnActor<AWCharacterBase>(PS->PlayerInfo.SelectedCharacter, PS->PlayerSpawner->GetActorLocation(), PS->PlayerSpawner->GetActorRotation());

				PC->OnPossess(RespawnChar);
					
				PS->SetHP(PS->GetMaxHP());
					
				UE_LOG(LogTemp, Log, TEXT("리스폰!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("리스폰 %s PlayerState 없음!"),*PlayerController->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("리스폰 %s 없음!"),*PlayerController->GetName());
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

void AWGameMode::OnNexusDestroyed(E_TeamID LoseTeam)
{
	if (!HasAuthority()) return;

	AWGameState* GS = GetGameState<AWGameState>();
	if (GS)
	{
		GS->SetGamePlay(E_GamePlay::GameEnded);
	}
//고쳐야함

	OnGameEnd.Broadcast();
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AWPlayerController* PC = Cast<AWPlayerController>(It->Get());
		if (PC)
		{
			PC->GameEnded(LoseTeam);
		}
	}
}

void AWGameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);
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
	AWPlayerController* WPlayerController = Cast<AWPlayerController>(Exiting);
	if (WPlayerController)
	{
		WGS->RemovePlayer(WPlayerController);	
	}
	
	Super::Logout(Exiting);
}