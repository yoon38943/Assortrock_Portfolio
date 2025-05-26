#include "Minions/MinionsSpawner.h"
#include "WMinionsCharacterBase.h"

AMinionsSpawner::AMinionsSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMinionsSpawner::BeginPlay()
{
	Super::BeginPlay();

	//StartSpawnMinions();	// 테스트 맵에서 테스트 끝나면 없앨 것
}

void AMinionsSpawner::StartSpawnMinions()
{
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
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::SpawnMinions, 30.f, false);
	}
}

