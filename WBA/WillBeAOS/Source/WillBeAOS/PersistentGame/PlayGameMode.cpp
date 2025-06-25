#include "PersistentGame/PlayGameMode.h"

#include "PlayGameState.h"


void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	GS = GetGameState<APlayGameState>();
	if (GS)
	{
		GS->SetGamePhase(EGamePhase::CharacterSelect);
	}
}

void APlayGameMode::StartLoading()
{
	if (GS)
	{
		GS->SetGamePhase(EGamePhase::LoadingPhase);
	}
}
