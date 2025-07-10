#include "Nexus.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "../Character/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PersistentGame/PlayGameMode.h"
#include "PersistentGame/PlayGameState.h"

ANexus::ANexus()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRootComponent);

	BoxCollisionComponet1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision1"));
	BoxCollisionComponet1->SetupAttachment(DefaultSceneRootComponent);
	BoxCollisionComponet2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision2"));
	BoxCollisionComponet2->SetupAttachment(DefaultSceneRootComponent);
	BoxCollisionComponet3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision3"));
	BoxCollisionComponet3->SetupAttachment(DefaultSceneRootComponent);

	NexusMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NexusMesh"));
	NexusMeshComponent->SetupAttachment(DefaultSceneRootComponent);

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	EndingCameraComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndingCamera"));
	EndingCameraComponent->SetupAttachment(DefaultSceneRootComponent);
	
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ANexus::DestroyNexus()
{
	NM_DestroyNexus();
}

void ANexus::NM_DestroyNexus_Implementation()
{
	FVector DestroypParticleLocation = NexusMeshComponent->GetComponentLocation();
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		DestroyParticle,
		FVector(DestroypParticleLocation.X, DestroypParticleLocation.Y, 400),
		FRotator::ZeroRotator,
		FVector(1.f),
		true);

	AActor* EndingCamera = GetWorld()->SpawnActor<AActor>(EndingCameraClass, EndingCameraComponent->GetComponentTransform(), FActorSpawnParameters());
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController && EndingCamera)
	{
		PlayerController->SetViewTargetWithBlend(EndingCamera, 1.f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
	}

	FTimerHandle DestroyTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle,
		[this]()
		{
			Destroy();
		},
		1.5f,
		false);
}

void ANexus::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->GameManagedActors.AddUnique(this);
		}
	}
}

void ANexus::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->GameManagedActors.Remove(this);
	}
}

float ANexus::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		float TakeDamage = DamageAmount;
		
		if (CombatComp != nullptr)
		{
			CombatComp->HandleTakeDamage(TakeDamage);

			if (CombatComp->GetIsDead())
			{
				APlayGameMode* GM = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
				if (GM)
				{
					GM->OnNexusDestroyed(TeamID);
				}

				DestroyNexus();
			}
		}

		return DamageAmount;
	}

	return 0;
}

float ANexus::GetNexusHPPercent()
{
	return CombatComp->Health/CombatComp->Max_Health;
}