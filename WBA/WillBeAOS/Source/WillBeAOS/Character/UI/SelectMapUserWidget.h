#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectMapUserWidget.generated.h"

enum class E_TeamID : uint8;

UCLASS()
class WILLBEAOS_API USelectMapUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void UpdateWidget();

	UFUNCTION(BlueprintCallable)
	void LogTeam(E_TeamID Team);
};
