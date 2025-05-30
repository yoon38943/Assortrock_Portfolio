// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

UCLASS()
class WILLBEAOS_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void ReturnToMainLobby();

	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnToMainLobbyButton;
};
