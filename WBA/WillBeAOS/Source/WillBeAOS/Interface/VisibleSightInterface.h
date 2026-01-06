#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "VisibleSightInterface.generated.h"

UINTERFACE(MinimalAPI)
class UVisibleSightInterface : public UInterface
{
	GENERATED_BODY()
};

class WILLBEAOS_API IVisibleSightInterface
{
	GENERATED_BODY()

public:
	virtual class UWidgetComponent* GetHPWidgetComponent() const = 0;
	
};
