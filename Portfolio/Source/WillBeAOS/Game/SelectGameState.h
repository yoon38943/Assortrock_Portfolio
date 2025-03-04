#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SelectGameState.generated.h"

#define SELECTTIME 20.0f;

UCLASS()
class WILLBEAOS_API ASelectGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	class ASelectPlayerController* SelectPC;
	
	UPROPERTY(ReplicatedUsing = OnRep_WidgetID, BlueprintReadWrite)
	int32 WidgetID;

	UPROPERTY(ReplicatedUsing = OnRep_UncheckWidgetID, BlueprintReadWrite)
	int32 UncheckWidgetID;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FText UserNickName;

	UPROPERTY(ReplicatedUsing = OnRep_SelectTime, BlueprintReadWrite)
	float SelectTime = SELECTTIME;
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_UpdateTeamSlot();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_UnCheckTeamSlot();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void M_UpdateCharName(int32 MWidgetID, const FText& MCharName);

	UFUNCTION(BlueprintCallable)
	void UpdateChar(int32 MWidgetID, const FText& MCharName);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_WidgetID();
	
	UFUNCTION()
	void OnRep_UncheckWidgetID();

	UFUNCTION()
	void OnRep_SelectTime();
	
};
