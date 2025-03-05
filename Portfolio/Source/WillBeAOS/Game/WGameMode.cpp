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
#include "Kismet/GameplayStatics.h"

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
	UE_LOG(LogTemp,Log,TEXT("PostLogin %s"),*NewPlayer->GetName());
	
	AWPlayerController* WPlayerController = Cast<AWPlayerController>(NewPlayer);
	if (WPlayerController)
	{
		PlayerControllers.AddUnique(WPlayerController);
		if (WGS)
		{
			SetGSPlayerControllers();
			WGS->CheckPlayerIsReady();
		}
	}
	Super::PostLogin(NewPlayer);
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
		if (It->TeamID == PlayerState->TeamID)
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
				WGS->AddTowerArray(SpawnedActor);
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
				
				APawn* Respawnpawn = GetWorld()->SpawnActor<APawn>(PS->SelectedPawnClass, PS->PlayerSpawner->GetActorLocation(), PS->PlayerSpawner->GetActorRotation());
				if (Respawnpawn)
				{
					AWCharacterBase* RespawnChar = Cast<AWCharacterBase>(Respawnpawn);
					if (RespawnChar)
					{
						UE_LOG(LogTemp, Log, TEXT("Player Spawner %s, %d"),*PC->GetName(),PS->TeamID);

						AChar_Wraith* RespawnWraith = Cast<AChar_Wraith>(RespawnChar);
						if (RespawnWraith)
						{
							PC->OnPossess(RespawnWraith);	
						}
						else
						{
							PC->OnPossess(RespawnChar);
						}
					}
					
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("RespawnChar is null!"));
					}
					UE_LOG(LogTemp, Log, TEXT("첫 스폰!"));
				}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("PawnClass is null!"));
					}
				}
				else
				{
					AWCharacterBase* RespawnChar = GetWorld()->SpawnActor<AWCharacterBase>(Player->GetClass(), PS->PlayerSpawner->GetActorLocation(), PS->PlayerSpawner->GetActorRotation());

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