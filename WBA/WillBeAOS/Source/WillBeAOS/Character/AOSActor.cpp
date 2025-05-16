#include "AOSActor.h"
#include "Net/UnrealNetwork.h"


AAOSActor::AAOSActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAOSActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAOSActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAOSActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TeamID);
	DOREPLIFETIME(ThisClass, GoldReward);
}

