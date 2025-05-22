#include "Function/WEnemyDetectorComponent.h"

#include "Kismet/GameplayStatics.h"


UWEnemyDetectorComponent::UWEnemyDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWEnemyDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(DetectionTimer, this, &UWEnemyDetectorComponent::DetectEnemies, 0.2f, true);
}

void UWEnemyDetectorComponent::DetectEnemies()
{
	if (!EnemyClass) return;

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, AllEnemies);

	AActor* Owner = GetOwner();
	if (!Owner) return;

	CurrentlyDetected.Empty();

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy || Enemy == Owner) continue;

		float Distance = FVector::Dist(Enemy->GetActorLocation(), Owner->GetActorLocation());
		if (Distance <= DetectionRadius)
		{
			CurrentlyDetected.Add(Enemy);
			OnEnemyDetected.Broadcast(Enemy);
		}
	}
}



