#include "Nexus.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "../Character/CombatComponent.h"
#include "../Game/WGameState.h"
#include "Net/UnrealNetwork.h"

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

void ANexus::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, NexusTeamID);
}

float ANexus::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	float TakeDamage = DamageAmount;
	if (CombatComp != nullptr)
	{
		CombatComp->HandleTakeDamage(TakeDamage);

		if ((CombatComp->GetIsDead()))
		{
			//DefaultSceneRootComponent->SetVisibility(false, true);
			AWGameState* WGameState = GetWorld()->GetGameState<AWGameState>();
			if (WGameState!=nullptr)
			{
				WGameState->HandleNexusDestroyed();
			}
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]() {Destroy(); GetWorld()->GetTimerManager().ClearTimer(TimerHandle); }, 1.5f, false);
		}
	}

	return DamageAmount;
}

float ANexus::GetNexusHPPercent()
{
	return CombatComp->Health/CombatComp->Max_Health;
}

void ANexus::BeginPlay()
{
	Super::BeginPlay();
	
}

