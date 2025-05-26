#include "PlayerSpawner.h"

#include "Net/UnrealNetwork.h"


APlayerSpawner::APlayerSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlayerSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TeamID);
}

