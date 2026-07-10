#include "Character/Wraith/Wraith_Projectile_Normal.h"

#include "KismetTraceUtils.h"
#include "Character/AOSActor.h"
#include "Character/Char_Wraith.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PersistentGame/GamePlayerController.h"
#include "PersistentGame/GamePlayerState.h"

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
