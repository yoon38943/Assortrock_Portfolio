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
	bool Test = true;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWMinionsCharacterBase> SpawnMinionsClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TrackNum;
	
	UPROPERTY(EditAnywhere)
	int32 SpawnCount = 0;
	UFUNCTION(Server, Reliable)
	void SpawnMinions();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	E_TeamID TeamID;//미니언의 팀 아이디
	
protected:
	virtual void BeginPlay() override;

};
