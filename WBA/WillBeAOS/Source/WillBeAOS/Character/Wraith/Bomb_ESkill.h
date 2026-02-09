#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bomb_ESkill.generated.h"

enum class E_TeamID : uint8;
class URotatingMovementComponent;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class WILLBEAOS_API ABomb_ESkill : public AActor
{
	GENERATED_BODY()

	float BombDamage = 0.f;

protected:
	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplosionEffect;

public:	
	ABomb_ESkill();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* CollisionComp;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BombMesh;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	URotatingMovementComponent* RotatingComponent;

	int64 UniqueID;

	E_TeamID TeamID;
	
	float CurrentRollValue = 0.f;


protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Explode(AActor* HitActor);
	void SetBombDamage(float Damage) { BombDamage = Damage; }
	
	void SpawnParticle();
};
