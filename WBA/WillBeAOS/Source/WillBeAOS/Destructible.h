#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "UObject/Interface.h"
#include "Destructible.generated.h"

UINTERFACE(MinimalAPI)
class UDestructible : public UInterface
{
	GENERATED_BODY()
};

class WILLBEAOS_API IDestructible
{
	GENERATED_BODY()

public:	//골드 관련
	virtual int32 GetGoldReward() const = 0;
	
public:	//팀 관련
	virtual E_TeamID GetTeamID() const = 0;
	virtual void SetTeamID(E_TeamID NewTeam) = 0;
};
