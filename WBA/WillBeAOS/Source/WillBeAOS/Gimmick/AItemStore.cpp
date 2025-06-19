#include "Gimmick/AItemStore.h"

#include "NavigationSystemTypes.h"
#include "Blueprint/UserWidget.h"
#include "Character/WCharacterBase.h"
#include "Character/WPlayerController.h"
#include "Character/WPlayerState.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AItemStore::AItemStore()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(DefaultSceneRoot);

	SPCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SPCollision"));
	SPCollision->SetupAttachment(GetRootComponent());

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

void AItemStore::BeginPlay()
{
	Super::BeginPlay();
}

void AItemStore::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!OtherActor) return;

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(OtherActor);
	if(!PlayerChar || StoreTeam != PlayerChar->CharacterTeam) return;
	
	GetWorld()->GetTimerManager().SetTimer(PlayerChar->HealingTimerHandle, [this, PlayerChar]()
	{
		if (HasAuthority())
		{
			AWPlayerState* PS = Cast<AWPlayerState>(PlayerChar->GetPlayerState());
			if (PS && PS->GetHP() < PS->GetMaxHP())
			{
				float HealAmount = PS->GetHP() + 50;
				if (HealAmount <= PS->GetMaxHP())
				{
					PS->SetHP(HealAmount);
				}
				else
				{
					PS->SetHP(PS->GetMaxHP());
				}
			}
		}
	}, 1.f, true);

	if (!HasAuthority())
	{
		AWPlayerController* PC = Cast<AWPlayerController>(GetWorld()->GetFirstPlayerController());
		APawn* Player = PC->GetPawn();

		if (OtherActor == Player)
		{
			PC->IsOpenedStore = true;
		}
	}
}

void AItemStore::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!OtherActor) return;

	AWCharacterBase* PlayerChar = Cast<AWCharacterBase>(OtherActor);
	if(!PlayerChar || StoreTeam != PlayerChar->CharacterTeam) return;

	if (PlayerChar->HealingTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerChar->HealingTimerHandle);
	}

	AWPlayerController* PC = Cast<AWPlayerController>(GetWorld()->GetFirstPlayerController());
	APawn* Player = PC->GetPawn();

	if (OtherActor == Player)
	{
		PC->IsOpenedStore = false;
	}
}