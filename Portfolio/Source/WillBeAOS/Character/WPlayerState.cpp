#include "Character/WPlayerState.h"

#include "WCharacterBase.h"
#include "Net/UnrealNetwork.h"

AWPlayerState::AWPlayerState()
{
    bReplicates = true;
    
    // Initialize default values for the player's stats
    HP = MaxHP;
    CPower = 50;
    CAdditionalHealth = 100;
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
}

float AWPlayerState::GetHPPercentage()
{
    return HP / MaxHP;
}

void AWPlayerState::Server_ApplyDamage_Implementation(int32 Damage)
{
    if (HP > 0)
    {
        if (HP > Damage)
            NM_SetHP(HP -= Damage);
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
}

bool AWPlayerState::Server_AddGold_Validate(int Amount)
{
    return Amount > 0;
}

void AWPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ThisClass, TeamID);
    DOREPLIFETIME(ThisClass, CGold);
    DOREPLIFETIME(ThisClass, HP);
}
