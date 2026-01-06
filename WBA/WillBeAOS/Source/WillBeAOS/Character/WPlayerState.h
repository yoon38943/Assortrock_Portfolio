#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "WEnumFile.h"
#include "GameFramework/PlayerState.h"
#include "WPlayerState.generated.h"

UCLASS()
class WILLBEAOS_API AWPlayerState : public APlayerState
{
	GENERATED_BODY()

/*protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(Replicated)
	bool bIsGameReady = false;

	UFUNCTION(Server, Reliable, WithValidation)
	void S_SetPlayerReady(bool bReady);

	// 유저 정보
	UPROPERTY(BlueprintReadOnly, Replicated)
	FPlayerInfoStruct PlayerInfo;

	// 유저 정보 받아오기
	UFUNCTION(Server, Reliable)
	void Server_TakePlayerInfo(const FString& PlayerName);

public:
	class APlayerSpawner* PlayerSpawner;
	
protected:
	UPROPERTY(Replicated)
	float HP;
	UPROPERTY(Replicated)
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
	void Server_ApplyDamage(int32 Damage, AController* AttackPlayer);
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

protected:
	// 플레이어 게임 정보
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerKillCount;
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerDeathCount;
	UPROPERTY(BlueprintReadOnly)
	float PlayerDamageAmount;

public:
	// Replicated가 너무 느려서 RPC로 변경
	UFUNCTION(NetMulticast, Reliable)
	void AddDeathPoint();
	UFUNCTION(NetMulticast, Reliable)
	void AddKillPoint();
	int32 GetKillPoints();
	int32 GetDeathPoints();
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;*/
};