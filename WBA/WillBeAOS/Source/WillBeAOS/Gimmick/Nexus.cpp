#include "Nexus.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "../Character/CombatComponent.h"
#include "Game/WGameMode.h"
#include "Kismet/GameplayStatics.h"

ANexus::ANexus()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRootComponent);

	BoxCollisionComponet = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollisionComponet->SetupAttachment(DefaultSceneRootComponent);

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

	NexusHP = 200;
	CombatComp->Health = NexusHP;
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
				AWGameMode* GM = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
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

