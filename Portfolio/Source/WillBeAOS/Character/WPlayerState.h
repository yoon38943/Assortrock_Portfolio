#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "GameFramework/PlayerState.h"
#include "WPlayerState.generated.h"

UCLASS()
class WILLBEAOS_API AWPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite,Replicated, Category = "Teams")
	E_TeamID TeamID;
	UPROPERTY(BlueprintReadWrite, Category = "Pawn")
	TSubclassOf<APawn> SelectedPawnClass;
	
	void SetTeamID(E_TeamID NewTeamID){TeamID = NewTeamID;}
	void SetPawnClass(TSubclassOf<APawn> SpawnClass) { SelectedPawnClass = SpawnClass; }

public:
	class APlayerSpawner* PlayerSpawner;
	
protected:
	float HP;
	float MaxHP;
	
public:
    AWPlayerState();

	UFUNCTION(BlueprintPure)
	float GetHP();
	UFUNCTION(BlueprintPure)
	float GetMaxHP();
	UFUNCTION(BlueprintCallable)
	void SetHP(int32 NewHP);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyDamage(int32 Damage);
	UFUNCTION(NetMulticast, Reliable)
	void NM_SetHP(float NewHP);

	UFUNCTION(BlueprintPure)
	float GetHPPercentage();
	
    // Attack power
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CPower;
	UFUNCTION(Server, Reliable)
	void AddPower(int32 Power);
	UFUNCTION(NetMulticast, Reliable)
	void C_SetPower(int32 NewPower);
    // Additional health
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CAdditionalHealth;
	UFUNCTION(Server, Reliable)
	void AddHealth(int32 Health);
	UFUNCTION(NetMulticast, Reliable)
	void C_SetHealth(int32 NewHP, int32 NewMaxHP, int32 NewAddHealth);
    // Defense power
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    float CDefense;
	UFUNCTION(Server, Reliable)
	void AddDefence(float Defence);
	UFUNCTION(NetMulticast, Reliable)
	void C_SetDefence(float NewDefence);
    // Movement speed
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    float CSpeed;
	UFUNCTION(Server, Reliable)
	void AddSpeed(float Speed);
	UFUNCTION(NetMulticast, Reliable)
	void C_SetSpeed(float NewSpeed);
    //Exp
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CCurrentExp;
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CExperience;
    // Level
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CLevel;
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CAbLevel;

	
	// 플레이어 골드 관련 스탯
	// Gold
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gold")
	int32 CGold;

	int Gold_Attack = 60;
	int Gold_Health = 150;
	int Gold_Defence = 30;
	int Gold_Speed = 30;

	// 서버에서 골드를 추가하는 함수
	UFUNCTION(Server, Reliable)
	void Server_AddGold(int Amount);
	UFUNCTION(Client, Reliable)
	void C_AddGold(int NewGold);
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};