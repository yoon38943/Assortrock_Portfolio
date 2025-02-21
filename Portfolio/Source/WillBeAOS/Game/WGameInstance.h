#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../WStructure.h"
#include "WGameInstance.generated.h"

UCLASS()
class WILLBEAOS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FString, int32> SavedPlayerTeams;  // Unique ID를 기반으로 팀 정보 저장

	UPROPERTY()
	TMap<FString,FPlayerValue> MatchPlayerTeams;
	
public:
	// GameInstance에 Key 값 저장
	void SavePlayerTeam(APlayerState* PlayerState, int32 TeamID);
	
	void SaveMatchPlayerTeam(FString PlayerName, int32 TeamID, TSubclassOf<class APawn> PawnClass);

	TMap<FString, int32> GetSavedTeam();
	
	TMap <FString, FPlayerValue> GetMatchTeam();
};
