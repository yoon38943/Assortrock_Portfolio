// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/SelectMapGameState.h"

#include "WGameInstance.h"
#include "Net/UnrealNetwork.h"

void ASelectMapGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;
	
	UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
	if (GI)
	{
		BlueTeamPlayersNum = GI->FinalBlueTeamPlayersNum;
		RedTeamPlayersNum = GI->FinalRedTeamPlayersNum;
	}
}

void ASelectMapGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASelectMapGameState, BlueTeamPlayersNum);
	DOREPLIFETIME(ASelectMapGameState, RedTeamPlayersNum);
}