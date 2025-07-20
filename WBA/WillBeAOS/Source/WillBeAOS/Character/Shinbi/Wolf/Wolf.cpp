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

	UE_LOG(LogTemp, Warning, TEXT("=== WOLF BeginPlay ==="));
	UE_LOG(LogTemp, Warning, TEXT("NetMode: %d (0=Standalone, 1=DedicatedServer, 2=Client)"), (int32)GetNetMode());
	UE_LOG(LogTemp, Warning, TEXT("HasAuthority: %s"), HasAuthority() ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("LocalRole: %d (0=None, 1=SimulatedProxy, 2=AutonomousProxy, 3=Authority)"), (int32)GetLocalRole());
	UE_LOG(LogTemp, Warning, TEXT("RemoteRole: %d"), (int32)GetRemoteRole());

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned Wolf In Server"));
	}
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned Wolf In Client"));
	}
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
	
	float Damage = 0;
	AController* Ctrl = Cast<AController>(GetOwner()->GetInstigatorController());
	if (Ctrl)
	{
		AGamePlayerState* PS = Ctrl->GetPlayerState<AGamePlayerState>();
		if (PS)
		{
			Damage = PS->CPower * 2.f;
		}
	}
	
	if (Damage > 0)
	{
		UGameplayStatics::ApplyPointDamage(
			OtherActor,
			Damage,
			GetActorForwardVector(),
			SweepResult,
			GetInstigatorController(),
			GetOwner(),
			UDamageType::StaticClass()
			);
	}
	UE_LOG(LogTemp, Warning, TEXT("Cntl: %s"), *GetInstigatorController()->GetName());
}

void AWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(FName("j_spine_3"));
	CollisionComponent->SetWorldLocation(SocketLocation);
}
