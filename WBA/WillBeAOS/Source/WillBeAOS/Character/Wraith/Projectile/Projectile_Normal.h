#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile_Normal.generated.h"

class UProjectileMovementComponent;

UCLASS()
class WILLBEAOS_API AProjectile_Normal : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ProjectileParticle;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
	
public:	
	AProjectile_Normal();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	FVector OwnerLocation;
	float TraceLength;
	float BulletSpeed;
	bool bReady = false;
};
