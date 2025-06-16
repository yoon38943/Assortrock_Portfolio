#include "Character/WPlayerState.h"

#include "WCharacterBase.h"
#include "Game/WGameMode.h"
#include "Game/WGameState.h"
#include "Net/UnrealNetwork.h"

void AWPlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority()) return;

    Server_TakePlayerInfo(GetPlayerName());
}

void AWPlayerState::Server_TakePlayerInfo_Implementation(const FString& PlayerName)
{
    // 게임 스테이트에서 플레이어 정보 받아오기
    AWGameState* GS = Cast<AWGameState>(GetWorld()->GetGameState());
    if (GS)
    {
        for (auto& Elem : GS->MatchPlayersInfo)
        {
            if (Elem.PlayerName == PlayerName)
            {
                PlayerInfo = Elem;
                UE_LOG(LogTemp, Log, TEXT("플레이어(%s) 정보 받아오기 완료!"), *PlayerName);
            }
        }

        // 정보 받아와서 캐릭터 소환
        AWGameMode* GM = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            GM->RespawnPlayer(nullptr, GetPlayerController());
        }
    }
}

void AWPlayerState::S_SetPlayerReady_Implementation(bool bReady)
{
    bIsGameReady = bReady;

    // 서버의 모든 플레이어가 준비가 되어있는지 확인
    AWGameState* GS = GetWorld()->GetGameState<AWGameState>();
    if (GS)
    {
        GS->CheckAllPlayersReady();
    }
}

bool AWPlayerState::S_SetPlayerReady_Validate(bool bReady)
{
    return true;
}

AWPlayerState::AWPlayerState()
{
    bReplicates = true;
    
    // Initialize default values for the player's stats
    MaxHP = 200;
    HP = MaxHP;
    CPower = 20;
    CAdditionalHealth = 0;
    CDefense = 5;
    CSpeed = 600.0f; // Unreal units per second
    CCurrentExp = 0;
    CExperience = 100;
    CLevel = 0;
    CAbLevel = 0;
    CGold = 0;
}

float AWPlayerState::GetHP()
{
    return HP;
}

float AWPlayerState::GetMaxHP()
{
    return MaxHP;
}

void AWPlayerState::SetHP(int32 NewHP)
{
    NM_SetHP(NewHP);
}

void AWPlayerState::NM_SetHP_Implementation(float NewHP)
{
    HP = NewHP;

    AWCharacterBase* Char = Cast<AWCharacterBase>(GetPawn());
    if (Char)
    {
        Char->SetHPPercentage();
    }
}

float AWPlayerState::GetHPPercentage()
{
    return HP / MaxHP;
}

void AWPlayerState::AddPower_Implementation(int32 Power)
{
    CPower += Power;
    C_SetPower(CPower);
}

void AWPlayerState::C_SetPower_Implementation(int32 NewPower)
{
    CPower = NewPower;
}

void AWPlayerState::AddHealth_Implementation(int32 Health)
{
    CAdditionalHealth += Health;

    HP += Health;
    MaxHP += Health;
    C_SetHealth(HP, MaxHP, CAdditionalHealth);
}

void AWPlayerState::C_SetHealth_Implementation(int32 NewHP, int32 NewMaxHP, int32 NewAddHealth)
{
    CAdditionalHealth = NewAddHealth;
    
    HP = NewHP;
    MaxHP = NewMaxHP;
}

void AWPlayerState::AddDefence_Implementation(float Defence)
{
    CDefense += Defence;
    C_SetDefence(CDefense);
}

void AWPlayerState::C_SetDefence_Implementation(float NewDefence)
{
    CDefense = NewDefence;
}

void AWPlayerState::AddSpeed_Implementation(float Speed)
{
    CSpeed += Speed;
    C_SetSpeed(CSpeed);
}

void AWPlayerState::C_SetSpeed_Implementation(float NewSpeed)
{
    CSpeed = NewSpeed;
}

void AWPlayerState::Server_ApplyDamage_Implementation(int32 Damage)
{
    if (HP > 0)
    {
        const float Reduction = CDefense / (CDefense + 100.f);
        const int32 FinalDamage = StaticCast<int32>(Damage * (1.f - Reduction));
        if (HP > FinalDamage)
            NM_SetHP(HP -= FinalDamage);
        else
            NM_SetHP(0);
        
        if (HP <= 0)
        {
            AWCharacterBase* PlayCharacter = Cast<AWCharacterBase>(GetPawn());
            if (PlayCharacter)
            {
                PlayCharacter->BeingDead();    // 캐릭터 사망처리
            }
        }
    }
}

bool AWPlayerState::Server_ApplyDamage_Validate(int32 Damage)
{
    return Damage >= 0;
}

void AWPlayerState::Server_AddGold_Implementation(int Amount)
{
    CGold += Amount;
    C_AddGold(CGold);
}

void AWPlayerState::C_AddGold_Implementation(int NewGold)
{
	CGold = NewGold;
}

void AWPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, bIsGameReady);
    DOREPLIFETIME(ThisClass, PlayerInfo);
    DOREPLIFETIME(ThisClass, HP);
    DOREPLIFETIME(ThisClass, MaxHP);
}
