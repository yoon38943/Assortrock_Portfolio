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

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void NM_HitEffect(const FVector& HitLocation);

public:
	E_TeamID TeamID;

	void DestroyProjectile(const FVector& StartLocation, const FVector& EndLocation);
};
