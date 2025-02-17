#include "WGameState.h"
#include "../Gimmick/Nexus.h"
#include "../Gimmick/Tower.h"
#include "../Character/WPlayerController.h"
#include "Character/WPlayerState.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Kismet/GameplayStatics.h"

void AWGameState::BeginPlay()
{
    Super::BeginPlay();
    
    CurrentGameState = E_GamePlay::GameInit;
    
    if (GEngine != nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Game State BeginPlay called"));
    }
    
    if (Nexus)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Found Nexus"));
    }

    GetTower();
    if (TowerArray.Num() > 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Found Towers"));
    }
    
    StartCountdown(5);
}

ANexus* AWGameState::GetNexus()
{
    ANexus* GameNexus;
    AActor* GetNexus = UGameplayStatics::GetActorOfClass(GetWorld(), ANexus::StaticClass());
    if (GetNexus)
    {
        GameNexus = Cast<ANexus>(GetNexus);
        return GameNexus;

    }
    return nullptr;
}

float AWGameState::GetNexusHP()
{
    if (Nexus != nullptr)   
    {
        return Nexus->GetNexusHPPercent();
        
    } return 0;
}

void AWGameState::GetTower()
{
    TArray<AActor*> GetTowers = {};
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(),GetTowers);
    if (GetTowers.Num() > 0)
    {
        for (AActor* Actor : GetTowers)
        {
            ATower* Tower = Cast<ATower>(Actor);
            if (Tower)
            {
                TowerArray.Add(Tower);
            }
        }
    }
}

int32 AWGameState::GetTowerNum()
{
    return TowerArray.Num();
}

void AWGameState::HandleNexusDestroyed()
{
    CurrentGameState = E_GamePlay::GameEnded;
    
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Nexus Destroyed!"));
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AWPlayerController* PC = Cast<AWPlayerController>(It->Get());
        if (PC)
        {
            PC->GameHasEnded(nullptr, true);
        }
    }
}

void AWGameState::AddPlayer(AWPlayerState* PlayerState)
{
    if (PlayerState)
    {
        ConnectedPlayerStates.Add(PlayerState);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Added"));
    }
}

void AWGameState::RemovePlayer(AWPlayerState* PlayerState)
{
    if (PlayerState)
    {
        ConnectedPlayerStates.Remove(PlayerState);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Removed"));
    }
}

void AWGameState::StartCountdown_Implementation(int32 InitialTime)
{
    CountdownTime = InitialTime;
    
    CurrentGameState = E_GamePlay::ReadyCountdown;
    
    GetWorldTimerManager().SetTimer(CountdownHandle, this, &AWGameState::UpdateCountdown, 1.f, true);
}

void AWGameState::UpdateCountdown()
{
    if (--CountdownTime <= 0)
    {
        GetWorldTimerManager().ClearTimer(CountdownHandle);
        CurrentGameState = E_GamePlay::Gameplaying;

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            if (AWPlayerController* PC = Cast<AWPlayerController>(It->Get()))
            {
                PC->OnGameStateChanged(CurrentGameState);
            }
        }
        return;
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("timer"));
}
