#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile_QSkill.generated.h"

class UProjectileMovementComponent;

UCLASS()
class WILLBEAOS_API AProjectile_QSkill : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ProjectileParticle;

	UPROPERTY(EditAnywhere, Category = "Particle")
	UParticleSystem* ProjectileTrail;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
	
public:	
	AProjectile_QSkill();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:	
	FVector OwnerLocation;
	FVector MuzzleLocation;
	FVector EndLocation;
	float TraceLength;
	float BulletSpeed;
	bool bReady = false;	
};
