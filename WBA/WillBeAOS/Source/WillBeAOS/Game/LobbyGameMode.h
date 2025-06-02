#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "WEnumFile.h"
#include "LobbyGameMode.generated.h"


UCLASS()
class WILLBEAOS_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	TMap<APlayerController*, E_TeamID> PlayersTeam;

	int32 BlueTeamNum = 0;
	int32 RedTeamNum = 0;

	bool IsServerTraveling = false;

public:
	void StartSelectCharacterMap();

	void AssignTeamToPlayer(APlayerController* Player);

	void UpdateTeamsInfoAfterAPlayerLeave(APlayerController* ExitingPlayer);
};
