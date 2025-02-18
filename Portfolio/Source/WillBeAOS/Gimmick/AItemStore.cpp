#include "Gimmick/AItemStore.h"

#include "NavigationSystemTypes.h"
#include "Blueprint/UserWidget.h"
#include "Character/WPlayerController.h"
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

	AWPlayerController* PC = Cast<AWPlayerController>(GetWorld()->GetFirstPlayerController());
	APawn* Player = PC->GetPawn();

	if (OtherActor == Player)
	{
		PC->IsOpenedStore = true;
	}
}

void AItemStore::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!OtherActor) return;

	AWPlayerController* PC = Cast<AWPlayerController>(GetWorld()->GetFirstPlayerController());
	APawn* Player = PC->GetPawn();

	if (OtherActor == Player)
	{
		PC->IsOpenedStore = false;
	}
}