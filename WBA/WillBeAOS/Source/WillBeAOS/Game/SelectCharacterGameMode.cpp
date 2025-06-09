#include "Game/SelectCharacterGameMode.h"

#include "WGameInstance.h"

void ASelectCharacterGameMode::StartInGame()
{
	UE_LOG(LogTemp, Warning, TEXT("인게임으로 이동!!!"));
	GetWorld()->ServerTravel("/Game/Portfolio/Menu/L_InGame?listen", true);
}