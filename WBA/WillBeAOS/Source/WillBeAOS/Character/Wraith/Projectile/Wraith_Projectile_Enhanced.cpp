#include "Character/Wraith/Wraith_Projectile_Enhanced.h"


void AWraith_Projectile_Enhanced::BeginPlay()
{
	Super::BeginPlay();

	ActorStartLocation = GetActorLocation();
}

void AWraith_Projectile_Enhanced::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float MoveDistance = FVector::Dist(GetActorLocation(), ActorStartLocation);
	if (MoveDistance >= DistanceVector)
	{
		Destroy();
	}
}
