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
}

void AWraith_Projectile_Normal::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWraith_Projectile_Normal::OnOverlapBegin);
	}
}

void AWraith_Projectile_Normal::Destroyed()
{
	Super::Destroyed();

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetActorLocation().ToString());
}

void AWraith_Projectile_Normal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && HasAuthority())
	{
		if (OtherActor == GetOwner()) return;
		
		AAOSCharacter* Character = Cast<AAOSCharacter>(OtherActor);
		if (Character)
		{
			if (Character->TeamID == TeamID) return;
		}
		AAOSActor* ObjActor = Cast<AAOSActor>(OtherActor);
		if (ObjActor)
		{
			if (ObjActor->TeamID == TeamID) return;
		}

		float CharacterDamage = 0;
		AGamePlayerController* PC = Cast<AGamePlayerController>(GetInstigatorController());
		if (PC)
		{
			AGamePlayerState* PState = PC->GetPlayerState<AGamePlayerState>();
			if (PState)
			{
				CharacterDamage = PState->CPower;
			}
		}

		if (Character || ObjActor)
		{
			UGameplayStatics::ApplyPointDamage(
				OtherActor,
				CharacterDamage,
				GetOwner()->GetActorForwardVector(),
				SweepResult,
				GetInstigatorController(),
				this,
				UDamageType::StaticClass()
			);
		}

		NM_HitEffect(SweepResult.ImpactPoint);
		Destroy();
	}
}

void AWraith_Projectile_Normal::NM_HitEffect_Implementation(const FVector& HitLocation)
{
	if (ImpactParticle && !HasAuthority())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticle,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(0.7f),
			true);
	}
}

void AWraith_Projectile_Normal::DestroyProjectile(const FVector& StartLocation, const FVector& EndLocation)
{
	float TimeToDestroy = (FVector::Dist(StartLocation, EndLocation) / ProjectileMovement_C->MaxSpeed);
	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, [this]()
	{
		Destroy();
	},
	TimeToDestroy,
	false);
}
