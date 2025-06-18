#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "GameFramework/Character.h"
#include "AOSCharacter.generated.h"

UCLASS()
class WILLBEAOS_API AAOSCharacter : public ACharacter, public IDestructible
{
	GENERATED_BODY()

public:
	AAOSCharacter();

	UPROPERTY(BlueprintReadWrite)
	class ATower* TowerWithCharacterInside;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	E_TeamID TeamID;

	virtual E_TeamID GetTeamID() const override{return TeamID;}
	virtual void SetTeamID(E_TeamID NewTeam) override{TeamID = NewTeam;}
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Gold")
	int32 GoldReward = 0;
	
	virtual int32 GetGoldReward() const {return GoldReward;};
	virtual void SetGoldReward(int32 NewGold){GoldReward = NewGold;}

	// 죽음 변수
	UPROPERTY(BlueprintReadWrite, Category = "Dead", Replicated)
	bool bIsDead = false;

	// 공격 록온 변수
	UPROPERTY(BlueprintReadWrite, Category = "LockOn")
	bool bIsEnemyLockOn;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
