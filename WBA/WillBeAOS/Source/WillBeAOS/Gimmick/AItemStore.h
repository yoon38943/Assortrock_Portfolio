#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "AItemStore.generated.h"

enum class E_TeamID : uint8;

UCLASS()
class WILLBEAOS_API AItemStore : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere)
	USphereComponent* SPCollision;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	bool IsShowStore = false;

	UPROPERTY(EditAnywhere)
	E_TeamID StoreTeam;
	
public:	
	AItemStore();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};