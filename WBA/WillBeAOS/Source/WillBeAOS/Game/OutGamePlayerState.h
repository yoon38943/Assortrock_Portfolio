// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerInfoStruct.h"
#include "OutGamePlayerState.generated.h"


UCLASS()
class WILLBEAOS_API AOutGamePlayerState : public APlayerState
{
	GENERATED_BODY()

	AOutGamePlayerState();

public:
	FPlayerInfoStruct PlayerInfo;
};
