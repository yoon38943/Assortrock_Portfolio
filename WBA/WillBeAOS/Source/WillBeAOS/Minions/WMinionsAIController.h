#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "WMinionsAIController.generated.h"

class UBehaviorTree;

UCLASS()
class WILLBEAOS_API AWMinionsAIController : public AAIController
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = BehaviorTree)
	UBehaviorTree* MinionBT;

	UPROPERTY(EditAnywhere, Category = "AIPerception")
	UAIPerceptionComponent* AIPerception;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCorrectPosition(FVector CorrectLocation);
	
private:
	void StartBTWithDelay();
};
