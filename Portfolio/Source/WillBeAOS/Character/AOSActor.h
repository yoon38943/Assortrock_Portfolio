#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "WEnumFile.h"
#include "GameFramework/Actor.h"
#include "AOSActor.generated.h"

UCLASS()
class WILLBEAOS_API AAOSActor : public AActor, public IDestructible
{
	GENERATED_BODY()
	
public:	
	AAOSActor();

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category = "Team")
	E_TeamID TeamID;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Gold")
	int32 GoldReward = 0;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual E_TeamID GetTeamID() const override{return TeamID;}
	virtual void SetTeamID(E_TeamID NewTeam) override{TeamID = NewTeam;}
	
	virtual int32 GetGoldReward() const {return GoldReward;};
	virtual void SetGoldReward(int32 NewGold){GoldReward = NewGold;}
};
