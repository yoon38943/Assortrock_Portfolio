#pragma once

#include "WEnumFile.h"
#include "PlayerInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfoStruct
{
	GENERATED_BODY()
	
	FString PlayerName;
	E_TeamID PlayerTeam;
	int32 PlayerTeamID;
	TSubclassOf<APawn> SelectedCharacter;
};