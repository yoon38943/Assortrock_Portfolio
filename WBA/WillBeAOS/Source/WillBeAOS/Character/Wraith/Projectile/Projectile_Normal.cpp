#include "Character/Wraith/Projectile_Normal.h"

#include "Character/Char_Wraith.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


AProjectile_Normal::AProjectile_Normal()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>("ProjectileParticle");
	RootComponent = ProjectileParticle;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
}

void AProjectile_Normal::BeginPlay()
{
	Super::BeginPlay();

	AChar_Wraith* wraith = Cast<AChar_Wraith>(GetOwner());
	if (wraith)
	{
		OwnerLocation = wraith->GetActorLocation();
		ProjectileMovement->InitialSpeed = BulletSpeed;
		bReady = true;
	}
}

void AProjectile_Normal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bReady)
	{
		float DistanceFromPlayer = FVector::DistSquared(GetActorLocation(), OwnerLocation);

		if (DistanceFromPlayer > TraceLength * TraceLength)
		{
			Destroy();
		}
	}
}
