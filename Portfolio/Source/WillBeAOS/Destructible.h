#pragma once

#include "CoreMinimal.h"
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

public:
	virtual int32 GetGoldReward() const = 0;
};
