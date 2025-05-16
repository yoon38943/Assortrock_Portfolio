#include "Gimmick/Projectile.h"

#include "Tower.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	CollisionComponent->InitSphereRadius(15);
	RootComponent = CollisionComponent;

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	Particle->SetupAttachment(GetRootComponent());
	Particle->bAutoActivate = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 1500.f;

	InitialLifeSpan = 5.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	ATower* Tower = Cast<ATower>(GetOwner());

	Target = Tower->TargetOfActors;
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 클라이언트 투사체 위치 보간
	if (!HasAuthority())  // 클라이언트에서만 실행
	{
		FVector TargetLocation = GetActorLocation();  // 서버에서 동기화된 위치
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 10.0f));
	}
}

void AProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (!HasAuthority()) return;

	if (Target == Other)
	{
		UGameplayStatics::ApplyDamage(
			Target,
			15,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

		Destroy();
	}
}

