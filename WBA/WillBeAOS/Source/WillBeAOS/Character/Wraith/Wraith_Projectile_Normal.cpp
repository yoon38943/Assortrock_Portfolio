#include "Character/Wraith/Wraith_Projectile_Normal.h"

#include "KismetTraceUtils.h"
#include "Character/AOSActor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PersistentGame/GamePlayerController.h"
#include "PersistentGame/GamePlayerState.h"

AWraith_Projectile_Normal::AWraith_Projectile_Normal()
{
	bReplicates = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionObjectType(ECC_Pawn);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);


	TracerComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	TracerComponent->SetupAttachment(BoxCollision);

	ProjectileMovement_C = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement_C"));
	ProjectileMovement_C->SetIsReplicated(false);
}

void AWraith_Projectile_Normal::BeginPlay()
{
	Super::BeginPlay();

	ActorStartLocation = GetActorLocation();

	/*
	if (HasAuthority())
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWraith_Projectile_Normal::OnOverlapBegin);
	}*/
}

void AWraith_Projectile_Normal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float MoveDistance = FVector::Dist(GetActorLocation(), ActorStartLocation);
	if (MoveDistance >= DistanceVector)
	{
		Destroy();
	}
}

void AWraith_Projectile_Normal::Destroyed()
{
	Super::Destroyed();
}