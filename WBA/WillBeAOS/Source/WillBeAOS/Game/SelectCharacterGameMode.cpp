#include "Game/SelectCharacterGameMode.h"

#include "WGameInstance.h"

void ASelectCharacterGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWGameInstance* GI = Cast<UWGameInstance>(GetGameInstance());
	if (GI)
	{
		for (auto& Info : GI->MatchPlayersTeamInfo)
		{
			UE_LOG(LogTemp, Log, TEXT("인스턴스 저장 플레이어 네임 : %s - 호출 완료(Select Character Map)"), *Info.Key);
		}
	}
}
