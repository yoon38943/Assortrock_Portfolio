#include "Game/OutGameState.h"
#include "OutPlayerController.h"
#include "Net/UnrealNetwork.h"


void AOutGameState::OnRep_PlayerCount()
{
	//추후 ui에 대기중인 숫자 변경
}

void AOutGameState::OnRep_IsMatched()
{
	ControllerIsReady();
}

void AOutGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AOutGameState::ControllerIsReady_Implementation()
{
	AOutPlayerController* PlayerController = Cast<AOutPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		PlayerController->IsMatched();
	}
}

void AOutGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOutGameState,IsMatched);
}
