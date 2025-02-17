#include "SpawnTowerPoint.h"
#include "Components/StaticMeshComponent.h"

ASpawnTowerPoint::ASpawnTowerPoint()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	PointMesh = CreateDefaultSubobject<UStaticMeshComponent>("SpawnTowerPoint");
	PointMesh->SetupAttachment(Root);
	FRotator NewRotation(0.0f, 270.0f, 0.0f); // Pitch, Yaw, Roll
	PointMesh->SetRelativeRotation(NewRotation);
}