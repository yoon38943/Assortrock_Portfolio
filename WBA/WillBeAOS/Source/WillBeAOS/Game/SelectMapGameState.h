#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "GameFramework/GameState.h"
#include "SelectMapGameState.generated.h"

UCLASS()
class WILLBEAOS_API ASelectMapGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// TMap은 Replicated가 안되기에 TArray로 교체
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_UpdateWidget)
	TArray<FPlayerInfoStruct> BlueTeamPlayerInfo;
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_UpdateWidget)
	TArray<FPlayerInfoStruct> RedTeamPlayerInfo;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 BlueTeamPlayersNum;
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 RedTeamPlayersNum;

	void AddSelectCharacterToPlayerInfo(const FString& PlayerName, TSubclassOf<APawn>& ChosenChar, E_TeamID& Team);

	UFUNCTION()
	void OnRep_UpdateWidget();
};
