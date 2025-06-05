#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectMapUserWidget.generated.h"

UCLASS()
class WILLBEAOS_API USelectMapUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void UpdateWidget();
};
