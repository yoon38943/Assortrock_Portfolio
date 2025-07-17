#include "Character/Shinbi/Wolf/Wolf.h"

#include "Chaos/AABBTree.h"
#include "Character/AOSActor.h"
#include "Character/AOSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PersistentGame/GamePlayerState.h"


AWolf::AWolf()
{
	PrimaryActorTick.bCanEverTick = true;

	RootCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = RootCollisionComponent;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootCollisionComponent);
	
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(SkeletalMeshComponent);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AWolf::OnOverlapBegin);

	SetReplicateMovement(true);
	bAlwaysRelevant = true;
}

void AWolf::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWolf::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	if (OtherActor == GetOwner()) return;
	AAOSActor* HitActor = Cast<AAOSActor>(OtherActor);
	AAOSCharacter* HitChar = Cast<AAOSCharacter>(OtherActor);
	if (!HitActor && !HitChar) return;
	if ((HitActor && HitActor->TeamID == TeamID) || (HitChar && HitChar->TeamID == TeamID)) return;
	
	if (HitActors.Contains(OtherActor)) return;
	HitActors.AddUnique(OtherActor);
	
	float Damage;
	AController* Ctrl = Cast<AController>(GetOwner()->GetInstigatorController());
	if (Ctrl)
	{
		AGamePlayerState* PS = Ctrl->GetPlayerState<AGamePlayerState>();
		if (PS)
		{
			Damage = PS->CPower * 1.5f;
		}
	}

	if (Damage)
	{
		UGameplayStatics::ApplyPointDamage(
			OtherActor,
			Damage,
			GetActorForwardVector(),
			SweepResult,
			GetOwner()->GetInstigatorController(),
			GetOwner(),
			UDamageType::StaticClass()
			);
	}
}

void AWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(FName("j_spine_3"));
	CollisionComponent->SetWorldLocation(SocketLocation);
}
