#include "PersistentGame/GamePlayerState.h"

#include "GamePlayerController.h"
#include "PlayGameMode.h"
#include "PlayGameState.h"
#include "Character/WCharacterBase.h"
#include "Game/WGameInstance.h"
#include "Net/UnrealNetwork.h"


void AGamePlayerState::StartCharacterSelectPhase()
{
	Server_ReplicatePlayerInfo(GetPlayerName());
}

void AGamePlayerState::OnRep_AddWidget()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetPlayerController());
	if (PC)
	{
		PC->PlayerStateInfoReady();
	}
}

void AGamePlayerState::StartInGamePhase()
{
	bReplicates = true;
    
	// Initialize default values for the player's stats
	MaxHP = 200;
	SetHP(MaxHP);
	CPower = 20;
	CAdditionalHealth = 0;
	CDefense = 5;
	CSpeed = 600.0f; // Unreal units per second
	CCurrentExp = 0;
	CExperience = 100;
	CLevel = 0;
	CAbLevel = 0;
	CGold = 0;
	
	if (HasAuthority()) return;

	Server_TakePlayerInfo(GetPlayerName());
}

float AGamePlayerState::GetHP()
{
	return HP;
}

float AGamePlayerState::GetMaxHP()
{
	return MaxHP;
}

void AGamePlayerState::SetHP(int32 NewHP)
{
	NM_SetHP(NewHP);
}

float AGamePlayerState::GetHPPercentage()
{
	return HP / MaxHP;
}

void AGamePlayerState::C_SetSpeed_Implementation(float NewSpeed)
{
	CSpeed = NewSpeed;
}

void AGamePlayerState::AddSpeed_Implementation(float Speed)
{
	CSpeed += Speed;
	C_SetSpeed(CSpeed);
}

void AGamePlayerState::C_SetDefence_Implementation(float NewDefence)
{
	CDefense = NewDefence;
}

void AGamePlayerState::AddDefence_Implementation(float Defence)
{
	CDefense += Defence;
	C_SetDefence(CDefense);
}

void AGamePlayerState::C_SetHealth_Implementation(int32 NewHP, int32 NewMaxHP, int32 NewAddHealth)
{
	CAdditionalHealth = NewAddHealth;
    
	HP = NewHP;
	MaxHP = NewMaxHP;
}

void AGamePlayerState::AddHealth_Implementation(int32 Health)
{
	CAdditionalHealth += Health;

	HP += Health;
	MaxHP += Health;
	C_SetHealth(HP, MaxHP, CAdditionalHealth);
}

void AGamePlayerState::C_SetPower_Implementation(int32 NewPower)
{
	CPower = NewPower;
}

void AGamePlayerState::AddPower_Implementation(int32 Power)
{
	CPower += Power;
	C_SetPower(CPower);
}

void AGamePlayerState::NM_SetHP_Implementation(float NewHP)
{
	HP = NewHP;

	AWCharacterBase* Char = Cast<AWCharacterBase>(GetPawn());
	if (Char)
	{
		Char->SetHPPercentage();
	}
}

void AGamePlayerState::Server_ApplyDamage_Implementation(int32 Damage, AController* AttackPlayer)
{
	if (HP > 0)
	{
		const float Reduction = CDefense / (CDefense + 100.f);
		const int32 FinalDamage = StaticCast<int32>(Damage * (1.f - Reduction));
		if (HP > FinalDamage)
		{
			if (AttackPlayer && Cast<AWCharacterBase>(AttackPlayer->GetPawn()))
			{
				AGamePlayerState* AttackPS = AttackPlayer->GetPlayerState<AGamePlayerState>();
				AttackPS->PlayerDamageAmount += FinalDamage;
			}
            
			NM_SetHP(HP -= FinalDamage);
		}
		else
		{
			if (AttackPlayer && Cast<AWCharacterBase>(AttackPlayer->GetPawn()))
			{
				AGamePlayerState* AttackPS = AttackPlayer->GetPlayerState<AGamePlayerState>();
				AttackPS->PlayerDamageAmount += HP;
			}
            
			NM_SetHP(0);
		}
        
		if (HP <= 0)
		{
			AddDeathPoint(); // 데스 카운트 +1
			if (IsValid(AttackPlayer))
			{
				if (AWCharacterBase* AttackChar = Cast<AWCharacterBase>(AttackPlayer->GetPawn()))
				{
					AttackPlayer->GetPlayerState<AGamePlayerState>()->AddKillPoint();

					if (APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState()))
					{
						GS->CheckKilledTeam(AttackChar->CharacterTeam);
					}
				}                    
			}
            
			AWCharacterBase* PlayCharacter = Cast<AWCharacterBase>(GetPawn());
			if (PlayCharacter)
			{
				PlayCharacter->BeingDead();    // 캐릭터 사망처리
			}
		}
	}
}

bool AGamePlayerState::Server_ApplyDamage_Validate(int32 Damage, AController* AttackPlayer)
{
	return Damage >= 0;
}

void AGamePlayerState::Server_TakePlayerInfo_Implementation(const FString& PlayerName)
{
	// 게임 스테이트에서 플레이어 정보 받아오기
	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		for (auto& Elem : GS->MatchPlayersInfo)
		{
			if (Elem.PlayerName == PlayerName)
			{
				InGamePlayerInfo = Elem;
				UE_LOG(LogTemp, Log, TEXT("플레이어(%s) 정보 받아오기 완료!"), *PlayerName);
			}
		}

		// 정보 받아와서 캐릭터 소환
		APlayGameMode* GM = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->RespawnPlayer(nullptr, GetPlayerController());
		}
	}
}

void AGamePlayerState::S_SetPlayerReady_Implementation(bool bReady)
{
	bIsGameReady = bReady;

	// 서버의 모든 플레이어가 준비가 되어있는지 확인
	APlayGameState* GS = GetWorld()->GetGameState<APlayGameState>();
	if (GS)
	{
		GS->CheckAllPlayersReady();
	}
}

bool AGamePlayerState::S_SetPlayerReady_Validate(bool bReady)
{
	return true;
}

void AGamePlayerState::Server_ReplicatePlayerInfo_Implementation(const FString& ClientPlayerName)
{
	UE_LOG(LogTemp, Log, TEXT("플레이어 네임 : %s"), *ClientPlayerName);
	
	PlayerInfo.PlayerName = *ClientPlayerName;

	UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
	if (GI)
	{
		UE_LOG(LogTemp, Log, TEXT("플레이어 네임 : %s - 맵 이동 완료(Select Character Map)"), *PlayerInfo.PlayerName);
		
		if (GI->MatchPlayersTeamInfo.Contains(PlayerInfo.PlayerName))
		{
			PlayerInfo = GI->GetSavedPlayerTeamInfo()[PlayerInfo.PlayerName];
			UE_LOG(LogTemp, Log, TEXT("플레이어 팀 정보 : %s"), PlayerInfo.PlayerTeam == E_TeamID::Blue? TEXT("블루팀") : TEXT("레드팀"));
		}
	}
}

void AGamePlayerState::Server_ChooseTheCharacter_Implementation(TSubclassOf<APawn> ChosenChar)
{
	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	{
		GS->AddSelectCharacterToPlayerInfo(GetPlayerName(), ChosenChar, PlayerInfo.PlayerTeam);
	}
}

void AGamePlayerState::Server_AddGold_Implementation(int Amount)
{
	CGold += Amount;
	C_AddGold(CGold);
}

void AGamePlayerState::C_AddGold_Implementation(int NewGold)
{
	CGold = NewGold;
}

void AGamePlayerState::AddDeathPoint_Implementation()
{
	PlayerDeathCount++;
}

void AGamePlayerState::AddKillPoint_Implementation()
{
	PlayerKillCount++;
}

int32 AGamePlayerState::GetKillPoints()
{
	return PlayerKillCount;
}

int32 AGamePlayerState::GetDeathPoints()
{
	return PlayerDeathCount;
}

void AGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGamePlayerState, PlayerInfo);
	DOREPLIFETIME(AGamePlayerState, InGamePlayerInfo);
	DOREPLIFETIME(AGamePlayerState, HP);
	DOREPLIFETIME(AGamePlayerState, MaxHP);
}
