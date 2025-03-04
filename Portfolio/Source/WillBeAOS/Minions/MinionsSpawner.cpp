#include "Minions/MinionsSpawner.h"

#include "WMinionsCharacterBase.h"
#include "Game/WGameMode.h"

AMinionsSpawner::AMinionsSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMinionsSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (Test)
	{
		GetWorld()->GetTimerManager().SetTimer(InitGameTimerHandle, this, &ThisClass::SpawnMinions, 1.f, false);
	}
}

void AMinionsSpawner::SpawnMinions_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(InitGameTimerHandle);
	
	SpawnCount++;
	
	AWMinionsCharacterBase* SpawnMinion = GetWorld()->SpawnActor<AWMinionsCharacterBase>(SpawnMinionsClass, GetActorLocation(), GetActorRotation());
	if (SpawnMinion)
	{
		SpawnMinion->TeamID = TeamID;
		SpawnMinion->TrackNum = TrackNum;
		SpawnMinion->SetTrackPoint();
		SpawnMinion->SpawnDefaultController();
		SpawnMinion->S_SetHPbarColor();
	}

	if (SpawnCount < 3)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::SpawnMinions, 1.f, false);
	}
	else
	{
		SpawnCount = 0;
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::SpawnMinions, 20.f, false);
	}
}

