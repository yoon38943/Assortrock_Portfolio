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
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement_C;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;


public:
	FVector OwnerLocation;
	float TraceLength;
	float BulletSpeed;
	bool bReady = false;

	void CalcFireBullet();
};
