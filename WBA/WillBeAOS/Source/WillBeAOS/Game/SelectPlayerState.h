#pragma once

#include "CoreMinimal.h"
#include "PlayerInfoStruct.h"
#include "GameFramework/PlayerState.h"
#include "SelectPlayerState.generated.h"

UCLASS()
class WILLBEAOS_API ASelectPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AddWidget)
	FPlayerInfoStruct PlayerInfo;

	UFUNCTION(Server, Reliable)
	void Server_ReplicatePlayerInfo(const FString& ClientPlayerName);

	UFUNCTION()
	void OnRep_AddWidget();
};
