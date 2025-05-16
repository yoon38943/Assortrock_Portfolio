#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class WILLBEAOS_API AProjectile : public AActor
{
	GENERATED_BODY()

	AActor* Target;

	UPROPERTY(EditAnywhere, Category = "Homing")
	float TurnSpeed = 1000.f;		// 조절 가능 회전 속도

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Particle")
	class UParticleSystemComponent* Particle;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
	class USceneComponent* HomingTargetComponent;

	UPROPERTY(ReplicatedUsing=OnRep_ChangeRotation)
	FRotator ReplicatedRotation;

	UFUNCTION()
	void OnRep_ChangeRotation();

	UPROPERTY(ReplicatedUsing = OnRep_Velocity)
	FVector ReplicatedVelocity;

	UFUNCTION()
	void OnRep_Velocity();
	
public:	
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void SetHomingTarget();
};
