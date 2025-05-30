#pragma once

#include "CoreMinimal.h"
#include "Character/AOSCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class WILLBEAOS_API AProjectile : public AActor
{
	GENERATED_BODY()

	AAOSCharacter* Target;

	UPROPERTY(EditAnywhere, Category = "Homing")
	float TurnSpeed = 1000.f;		// 조절 가능 회전 속도

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	USphereComponent* CollisionComponent;

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
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
);

	void SetHomingTarget();
};
