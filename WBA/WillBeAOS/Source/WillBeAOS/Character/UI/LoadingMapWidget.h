#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingMapWidget.generated.h"

UCLASS()
class WILLBEAOS_API ULoadingMapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	float LoadPercentage;

public:
	void SetLoadPercentage(const float Percentage);
};
