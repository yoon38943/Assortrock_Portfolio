#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SelectGameState.generated.h"


UCLASS()
class WILLBEAOS_API ASelectGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	class ASelectPlayerController* SelectPC;

	UPROPERTY(ReplicatedUsing = OnRep_SelectTime, BlueprintReadWrite)
	float SelectTime = 20;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_UpdateTeamSlot(int32 WidgetID, const FText& UserNickName);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_UnCheckTeamSlot(int32 UnCheckWidgetID);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_UpdateCharName(int32 MWidgetID, const FText& MCharName);

	UFUNCTION(BlueprintCallable)
	void UpdateChar(int32 MWidgetID, const FText& MCharName);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_SelectTime();
	
};
