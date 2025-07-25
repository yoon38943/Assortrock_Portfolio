#include "Character/Wraith/Wraith_Projectile_Normal.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AWraith_Projectile_Normal::AWraith_Projectile_Normal()
{
	bReplicates = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
}

void AWraith_Projectile_Normal::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			BoxCollision,
			FName(""),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
			);
	}

	if (HasAuthority())
	{
		BoxCollision->OnComponentHit.AddDynamic(this, &AWraith_Projectile_Normal::OnHit);
	}
}

void AWraith_Projectile_Normal::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorLocation());
	}
}

void AWraith_Projectile_Normal::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}
