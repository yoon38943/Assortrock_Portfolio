#pragma once

#include "CoreMinimal.h"
#include "WMinionsCharacterBase.h"
#include "GameFramework/Actor.h"
#include "MinionsSpawner.generated.h"

UCLASS()
class WILLBEAOS_API AMinionsSpawner : public AActor
{
	GENERATED_BODY()
	FTimerHandle InitGameTimerHandle;
	FTimerHandle SpawnTimerHandle;
	
public:	
	AMinionsSpawner();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWMinionsCharacterBase> SpawnMinionsClass;
	
	UPROPERTY(EditAnywhere)
	int32 SpawnCount = 0;
	UFUNCTION(Server, Reliable)
	void SpawnMinions();
	
protected:
	virtual void BeginPlay() override;

};
