#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/WPlayerController.h"
#include "ItemStoreWidget.generated.h"

UCLASS()
class WILLBEAOS_API UItemStoreWidget : public UUserWidget
{
	GENERATED_BODY()

	AWPlayerController* PC;
	AWPlayerState* PS;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void AddPowerState();
	
	UFUNCTION(BlueprintCallable)
	void AddHealthState();

	UFUNCTION(BlueprintCallable)
	void AddDefenceState();

	UFUNCTION(BlueprintCallable)
	void AddSpeedState();
};
