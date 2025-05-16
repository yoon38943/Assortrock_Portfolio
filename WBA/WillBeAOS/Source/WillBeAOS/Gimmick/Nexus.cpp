#include "Nexus.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "../Character/CombatComponent.h"
#include "Game/WGameMode.h"

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
	
	bReplicates = true; 
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
				
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]() {Destroy(); GetWorld()->GetTimerManager().ClearTimer(TimerHandle); }, 1.5f, false);
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

