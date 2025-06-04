// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SelectMapGameState.generated.h"

/**
 * 
 */
UCLASS()
class WILLBEAOS_API ASelectMapGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 BlueTeamPlayersNum;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 RedTeamPlayersNum;
};
