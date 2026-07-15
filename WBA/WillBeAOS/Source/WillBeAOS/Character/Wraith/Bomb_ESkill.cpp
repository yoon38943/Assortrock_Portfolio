#include "Character/Wraith/Bomb_ESkill.h"

#include "Character/AOSCharacter.h"
#include "Char_Wraith.h"
#include "Components/SphereComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PersistentGame/PlayGameState.h"


ABomb_ESkill::ABomb_ESkill()
{
	//PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = CollisionComp;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(CollisionComp);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootComponent);
	ProjectileMovement->bRotationFollowsVelocity = true;

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	RotatingComponent->RotationRate = FRotator(-720.0f, 0.0f, 720.0f);
}

void ABomb_ESkill::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ABomb_ESkill::OnHit);
	}

	SetLifeSpan(10.0f);
}

void ABomb_ESkill::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                         FVector NormalImpulse, const FHitResult& Hit)
{	
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		Explode(OtherActor);
	}
}

void ABomb_ESkill::Explode(AActor* HitActor)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(GetOwner());

	if (APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState()))
	{
		for (auto WeakActor : GS->GameManagedActors)
		{
			if (WeakActor)
			{
				AActor* Ally = WeakActor;
			
				if (!Ally) continue;
		
				// AAOSCharacter 중 아군 채널 제외
				AAOSCharacter* InGameChar = Cast<AAOSCharacter>(Ally);
				if (InGameChar)
				{
					if (InGameChar->TeamID == TeamID)
					{
						IgnoreActors.Add(Ally);
					}
				}

				// AAOSActor 중 아군 채널 제외
				AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
				if (InGameActor)
				{
					if (IsValid(InGameActor) && InGameActor->TeamID == TeamID)
					{
						IgnoreActors.Add(Ally);
					}
				}
			}
		}
	}
	
	UGameplayStatics::ApplyRadialDamage(
		this,
		BombDamage,
		GetActorLocation(),
		200.f,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetInstigatorController(),
		true // 가로막는 물체가 있어도 데미지를 줄지 여부
	);

	UE_LOG(LogTemp, Log, TEXT("Explode at : %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));

	AChar_Wraith* WraithChar = Cast<AChar_Wraith>(GetOwner());
	if (WraithChar)
	{
		WraithChar->Multicast_ExplodeBomb(UniqueID);
	}
	Destroy();
}

void ABomb_ESkill::SpawnParticle()
{
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation(),
			FVector(0.8f),
			true,
			EPSCPoolMethod::AutoRelease,
			true
		);
	}
}