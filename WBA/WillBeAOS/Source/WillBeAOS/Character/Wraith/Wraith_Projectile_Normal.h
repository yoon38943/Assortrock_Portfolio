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
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;
	
	UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
