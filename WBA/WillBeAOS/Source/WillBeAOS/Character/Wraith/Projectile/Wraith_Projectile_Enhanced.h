#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Projectile.h"
#include "Wraith_Projectile_Enhanced.generated.h"

UCLASS()
class WILLBEAOS_API AWraith_Projectile_Enhanced : public AProjectile
{
	GENERATED_BODY()

	FVector ActorStartLocation;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
public:
	float DistanceVector;
};
