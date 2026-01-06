#include "WGameState.h"
#include "WGameInstance.h"
#include "WGameMode.h"
#include "../Gimmick/Nexus.h"
#include "../Gimmick/Tower.h"
#include "../Character/WPlayerController.h"
#include "Character/WPlayerState.h"
#include "Net/UnrealNetwork.h"

class UWGameInstance;

void AWGameState::NM_ReplicateTotalKillPoints_Implementation(int32 Blue, int32 Red)
{
    BlueTeamTotalKillPoints = Blue;
    RedTeamTotalKillPoints = Red;

    DelegateShowKillState.ExecuteIfBound(BlueTeamTotalKillPoints, RedTeamTotalKillPoints);
}

void AWGameState::CheckKilledTeam(E_TeamID KillTeam)
{
    if (KillTeam == E_TeamID::Blue)
    {
        BlueTeamTotalKillPoints++;
    }
    else if (KillTeam == E_TeamID::Red)
    {
        RedTeamTotalKillPoints++;
    }

    NM_ReplicateTotalKillPoints(BlueTeamTotalKillPoints, RedTeamTotalKillPoints);
}

void AWGameState::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(RespawnTimeHandle, this, &ThisClass::AddRespawnTime, 300.f, true);
    }

    WGameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
    if (WGameMode == nullptr)
    {
        GetWorld()->GetTimerManager().SetTimer(GetGMHandle, this, &AWGameState::UpdateGMTimer, 0.3f, true);
    }
    else
    {
        SetGamePlay(E_GamePlay::GameInit);
    }
}

void AWGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (RespawnTimeHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(RespawnTimeHandle);
    }
}

void AWGameState::UpdateGMTimer()
{
    WGameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
    if (WGameMode != nullptr)
    {
        GetWorldTimerManager().ClearTimer(GetGMHandle);
    }
}

void AWGameState::SetGamePlay(E_GamePlay NewState)
{
    if (CurrentGameState == NewState) {return;}
    
    CurrentGameState = NewState;
    UE_LOG(LogTemp, Log, TEXT("GameRunning %s"),*UEnum::GetValueAsString(CurrentGameState));

    GamePlayStateChanged(CurrentGameState);
}

void AWGameState::GamePlayStateChanged(E_GamePlay NewState)
{
    switch (NewState)
    {
    case E_GamePlay::GameInit:
        UE_LOG(LogTemp, Log, TEXT("Game is initializing..."));
        TakeMatchPlayersInfoToInstance();
        break;

    case E_GamePlay::PlayerReady://플레이어 컨트롤러 정보를 다 받아와서
        UE_LOG(LogTemp, Log, TEXT("All Players are ready! Updating PlayerControllers..."));
        break;

    case E_GamePlay::ReadyCountdown:
        UE_LOG(LogTemp, Log, TEXT("Countdown before game starts!"));
        ServerCountdown();
        break;

    case E_GamePlay::Gameplaying:
        UE_LOG(LogTemp, Log, TEXT("Game has started!"));
        //미니언 스폰
        SetGameStart();
        break;

    case E_GamePlay::GameEnded://넥서스 파괴 후
        UE_LOG(LogTemp, Log, TEXT("Game has ended!"));
        break;
    }
}

//-----------------------------------------------------------
//인스턴스에서 플레이어스테이트 맵 가져오기
//-----------------------------------------------------------
void AWGameState::TakeMatchPlayersInfoToInstance()
{
    if (!HasAuthority()) return;
    
    UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
    if (GI)
    {
        for (auto& Elem : GI->MatchPlayersTeamInfo)
        {
            MatchPlayersInfo.Add(Elem.Value);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("게임 스테이트에 매치 플레이어 정보 넘기기 완료!(InGameMap)"));
}

void AWGameState::CheckPlayerIsReady()
{
    if (IsAllPlayerIsReady())
    {
        SetGamePlay(E_GamePlay::PlayerReady);
    }
}

bool AWGameState::IsAllPlayerIsReady()
{
    TArray<AWPlayerState*> AllPlayerStates;

    int32 NumPlayers = 0;
    
    for (auto& It : PlayerControllers)
    {
        if (AWPlayerState* PS = Cast<AWPlayerState>(It->PlayerState))
        {
            AllPlayerStates.Add(PS);
            UE_LOG(LogTemp, Log, TEXT("GameState PlayerStateAdd %s"), *PS->GetPlayerName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerState not found or cast failed."));
        }
    }
    
    for (auto PS : AllPlayerStates)
    {
        for (auto It : MatchPlayersInfo)
        {
            if (PS)// 특정 이름 패턴을 가진 경우만 처리
            {
                if (PS->GetPlayerName() == It.PlayerName)
                {
                    UE_LOG(LogTemp, Log, TEXT("플레이어 스테이트 네임 : %s"), *PS->GetPlayerName());
                    UE_LOG(LogTemp, Log, TEXT("게임 스테이트에 저장된 네임 : %s"), *It.PlayerName);
                    
                    ConnectedPlayerStates.Add(PS);
                    NumPlayers++;
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("PlayerState is empty."));
            }
        }
    }

    if (NumPlayers == MatchPlayersInfo.Num())
    {
        return true;
    }
    return false;
}

void AWGameState::CheckPlayerSpawned(AWPlayerController* WPlayerController)
{
    UE_LOG(LogTemp, Log, TEXT("CheckPlayerSpawned %s"),*WPlayerController->GetName());
    CheckSpawnedPlayers++;
    
    if (CheckSpawnedPlayers == MatchPlayersInfo.Num())
    {
        SetGamePlay(E_GamePlay::ReadyCountdown);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("All Players are not Spawned"));
    }
}

void AWGameState::CheckAllPlayersReady()
{
    bool bAllReady = true;

    for (APlayerState* PS : PlayerArray) // 모든 플레이어 상태 확인
    {
        AWPlayerState* WPS = Cast<AWPlayerState>(PS);
        if (WPS && !WPS->bIsGameReady) // 아직 준비 안된 플레이어가 있다면
        {
            bAllReady = false;
            break;
        }
    }

    if (bAllReady)
    {
        CheckPlayerIsReady();
    }
}


void AWGameState::RemovePlayer(AWPlayerController* WPlayerController)
{
    if (WPlayerController)
    {
        PlayerControllers.Remove(WPlayerController);
        ConnectedPlayerStates.Remove(WPlayerController->GetPlayerState<AWPlayerState>());
        UE_LOG(LogTemp,Warning,TEXT("Player Removed"));
    }
}

void AWGameState::ServerCountdown()
{
    WGameMode->SetGSPlayerControllers();
    WGameMode->StartCountdown(11);
}

void AWGameState::SetCountdownTime(int32 NewCount)
{
    CountdownTime = NewCount;
    for (auto It :PlayerControllers)
    {
        It->CountdownTime = NewCount;
    }
}

void AWGameState::SetGameStart()
{
    for (auto& It : PlayerControllers)
    {
        It->OnGameStateChanged(E_GamePlay::Gameplaying);
    }
    
    //벽 파괴
    WGameMode->DestroyWall();
    //미니언 스폰
    WGameMode->SpawnMinions();
}

void AWGameState::AddRespawnTime()
{
    RespawnTime += 5;
}

void AWGameState::AssignNexus(AAOSActor* SpawnedActor)
{
    if (ANexus* WNexus = Cast<ANexus>(SpawnedActor))
    {
        if (WNexus->TeamID == E_TeamID::Red)
        {
            RedNexus = WNexus;
        }
        else if (WNexus->TeamID == E_TeamID::Blue)
        {
            BlueNexus = WNexus;
        }
    }
}


float AWGameState::GetBlueNexusHP()
{
    if (BlueNexus)   
    {
        return BlueNexus->GetNexusHPPercent();
        
    } return 0;
}

float AWGameState::GetRedNexusHP()
{
    if (RedNexus)   
    {
        return RedNexus->GetNexusHPPercent();
        
    } return 0;
}

void AWGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ThisClass,BlueNexus);
    DOREPLIFETIME(ThisClass,RedNexus);
    DOREPLIFETIME(ThisClass,RespawnTime);
}