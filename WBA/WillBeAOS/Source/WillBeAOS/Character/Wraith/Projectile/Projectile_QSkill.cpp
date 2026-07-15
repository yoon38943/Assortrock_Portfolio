#include "Projectile_QSkill.h"
#include "../Char_Wraith.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AProjectile_QSkill::AProjectile_QSkill()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileParticle = CreateDefaultSubobject<UParticleSystemComponent>("ProjectileParticle");
	RootComponent = ProjectileParticle;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
}

void AProjectile_QSkill::BeginPlay()
{
	Super::BeginPlay();

	AChar_Wraith* wraith = Cast<AChar_Wraith>(GetOwner());
	if (wraith)
	{
		OwnerLocation = wraith->GetActorLocation();
		ProjectileMovement->InitialSpeed = BulletSpeed;
		bReady = true;

		if (ProjectileTrail)
		{
			float Distance = FVector::Dist(EndLocation, MuzzleLocation);
			FRotator SpawnRotator = (EndLocation - MuzzleLocation).Rotation();
			
			UParticleSystemComponent* SpawnTrail = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ProjectileTrail,
				EndLocation,
				SpawnRotator
			);

			if (SpawnTrail)
			{
				float ScaleX = Distance / 100.f;
				SpawnTrail->SetRelativeScale3D(FVector(-ScaleX, 1.5f, 1.5f));
			}
		}
	}
}

void AProjectile_QSkill::Tick(float DeltaTime)
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