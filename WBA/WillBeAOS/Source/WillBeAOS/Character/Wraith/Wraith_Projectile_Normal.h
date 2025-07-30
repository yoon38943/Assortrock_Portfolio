#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Projectile.h"
#include "Wraith_Projectile_Normal.generated.h"

UCLASS()
class WILLBEAOS_API AWraith_Projectile_Normal : public AProjectile
{
	GENERATED_BODY()

	AWraith_Projectile_Normal();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement_C;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	FVector ActorStartLocation;


public:
	E_TeamID TeamID;

	bool CanHit;
	
	float DistanceVector;
};
