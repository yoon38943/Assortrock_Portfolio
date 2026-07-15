#include "Wraith_Projectile_Normal.h"
#include "../Char_Wraith.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


AWraith_Projectile_Normal::AWraith_Projectile_Normal()
{
	TracerComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	TracerComponent->SetupAttachment(BoxCollision);

	ProjectileMovement_C = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement_C"));
}

void AWraith_Projectile_Normal::Tick(float DeltaTime)
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

void AWraith_Projectile_Normal::CalcFireBullet()
{
	AChar_Wraith* wraith = Cast<AChar_Wraith>(GetOwner());
	if (wraith)
	{
		OwnerLocation = wraith->GetActorLocation();
		ProjectileMovement_C->InitialSpeed = BulletSpeed;
		bReady = true;
	}
}
