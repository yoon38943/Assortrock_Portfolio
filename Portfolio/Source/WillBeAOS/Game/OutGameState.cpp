#include "Game/OutGameState.h"
#include "Net/UnrealNetwork.h"


void AOutGameState::OnRep_PlayerCount()
{
	//추후 ui에 대기중인 숫자 변경
}

void AOutGameState::OnRep_IsMatched()
{
	Controll();
}

void AOutGameState::Controll_Implementation()
{
}

void AOutGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOutGameState, CurrentPlayerCount);
}
