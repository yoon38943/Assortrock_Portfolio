#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "GameFramework/Actor.h"
#include "PlayerSpawner.generated.h"

UCLASS()
class WILLBEAOS_API APlayerSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerSpawner();

	UPROPERTY(Replicated, EditAnywhere,BlueprintReadOnly, Category = "PlayerSpawner")
	E_TeamID TeamID;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
