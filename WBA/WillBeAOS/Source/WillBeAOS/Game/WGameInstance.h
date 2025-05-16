#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../WStructure.h"
#include "WGameInstance.generated.h"

enum class E_TeamID : uint8;

UCLASS()
class WILLBEAOS_API UWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FString, E_TeamID> SavedPlayerTeams;  // Unique ID를 기반으로 팀 정보 저장

	UPROPERTY()
	TMap<FString,FPlayerValue> MatchPlayerTeams;
	
public:
	// GameInstance에 Key 값 저장
	void SavePlayerTeam(APlayerState* PlayerState, E_TeamID TeamID);
	
	void SaveMatchPlayerTeam(FString PlayerName, E_TeamID, TSubclassOf<class APawn> PawnClass);

	TMap<FString, E_TeamID> GetSavedTeam();
	
	TMap <FString, FPlayerValue> GetMatchTeam();
};