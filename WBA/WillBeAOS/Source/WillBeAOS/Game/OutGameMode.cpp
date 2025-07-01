#include "OutGameMode.h"
#include "OutGameState.h"


AOutGameMode::AOutGameMode()
{	
	bUseSeamlessTravel = true;
}

void AOutGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	PlayerReadyStatus.Add(NewPlayer, false);
	
	AOutGameState* OutGameState = GetGameState<AOutGameState>();
	if (OutGameState)
	{
		OutGameState->PlayerControllers.Add(Cast<AOutPlayerController>(NewPlayer));
	}
}

void AOutGameMode::ChangeToNextMap()
{
	if (SelectMap.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectMap is not set in GameMode!"));
		return;
	}
	
	FString NextMap = SelectMap.GetAssetName();
	
	//서버가 같은 맵으로 이동시키는 함수
	GetWorld()->ServerTravel(NextMap);
}

void AOutGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

