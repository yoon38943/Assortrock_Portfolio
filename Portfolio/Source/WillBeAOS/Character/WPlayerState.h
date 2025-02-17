#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "GameFramework/PlayerState.h"
#include "WPlayerState.generated.h"

UCLASS()
class WILLBEAOS_API AWPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(Replicated)
	float HP;
	UPROPERTY(Replicated)
	float MaxHP = 100;
	
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
    // Additional health
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CAdditionalHealth;
    // Defense power
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    int32 CDefense;
    // Movement speed
    UPROPERTY(BlueprintReadWrite, Category = "Stats")
    float CSpeed;
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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Gold")
	int32 CGold;

	// 서버에서 골드를 추가하는 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddGold(int Amount);
public:
	UPROPERTY(BlueprintReadWrite,Replicated, Category = "Teams")
	E_TeamID TeamID;
	
	void SetTeamID(E_TeamID NewTeamID){TeamID = NewTeamID;}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
