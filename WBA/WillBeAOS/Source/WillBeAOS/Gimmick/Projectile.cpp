#include "Gimmick/Projectile.h"

#include "Tower.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"


AProjectile::AProjectile()
{
	bReplicates = true;
	SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	CollisionComponent->InitSphereRadius(15);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComponent;
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnComponentBeginOverlap);
	CollisionComponent->SetGenerateOverlapEvents(true);

	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	Particle->SetupAttachment(GetRootComponent());
	Particle->bAutoActivate = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;		// 발사체 지정(없으면 무엇을 발사할지 인식 못함)
	ProjectileMovement->InitialSpeed = 1200.f;
	ProjectileMovement->bIsHomingProjectile = true;		// 발사체 유도 기능
	ProjectileMovement->HomingAccelerationMagnitude = 3000.f;	// 발사체 유도 민감도
	ProjectileMovement->SetIsReplicated(true);


	InitialLifeSpan = 5.f;	// 발사체 존재 가능 시간
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && Target)
	{
		FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
		ProjectileMovement->Activate();
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 투사체 위치 보간
	if (!HasAuthority() || !Target) return;

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	// 현재 속도 방향
	FVector CurrentVelocity = ProjectileMovement->Velocity;
	FVector DesiredDirection = (TargetLocation - CurrentLocation).GetSafeNormal();

	// 방향 보간
	FVector NewDirection = FMath::VInterpNormalRotationTo(
		CurrentVelocity.GetSafeNormal(),
		DesiredDirection,
		DeltaTime,
		TurnSpeed
	);

	// 새로운 속도 적용
	ProjectileMovement->Velocity = NewDirection * ProjectileMovement->InitialSpeed;

	if (!ProjectileMovement->Velocity.IsNearlyZero())
	{
		ReplicatedRotation = ProjectileMovement->Velocity.GetSafeNormal().Rotation();
		ReplicatedVelocity = ProjectileMovement->Velocity;
		SetActorRotation(ReplicatedRotation);
	}

	// 날아가는 와중 타겟이 죽거나 사라졌을 경우
	if (HasAuthority() && !IsValid(Target) || Target->bIsDead == true)
	{
		Target = nullptr;
		Destroy();
	}
}

void AProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (Target == OtherActor)
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

void AProjectile::SetHomingTarget()
{
	ATower* Tower = Cast<ATower>(GetOwner());

	if (!Tower)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile has no valid tower owner!"));
		return;
	}
	
	Target = Tower->TargetOfActors;
	HomingTargetComponent = Target->GetRootComponent();

	if (!Target || !HomingTargetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Homing target invalid!"));
		return;
	}
	
	ProjectileMovement->HomingTargetComponent = HomingTargetComponent;
}

void AProjectile::OnRep_ChangeRotation()
{
	// 클라이언트에서 회전 동기화
	SetActorRotation(ReplicatedRotation);
}

void AProjectile::OnRep_Velocity()
{
	ProjectileMovement->Velocity = ReplicatedVelocity;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, ReplicatedRotation);
	DOREPLIFETIME(AProjectile, ReplicatedVelocity);
}