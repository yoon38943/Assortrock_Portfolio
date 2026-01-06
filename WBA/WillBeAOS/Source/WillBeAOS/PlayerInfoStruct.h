#pragma once

#include "WEnumFile.h"
#include "PlayerInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName = "";
	UPROPERTY(BlueprintReadOnly)
	FString PlayerNickName = "";
	UPROPERTY(BlueprintReadOnly)
	E_TeamID PlayerTeam = E_TeamID::Neutral;
	UPROPERTY(BlueprintReadOnly)
	int32 PlayerTeamID = 0;
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<APawn> SelectedCharacter = nullptr;
};