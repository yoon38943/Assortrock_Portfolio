#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "AItemStore.generated.h"

UCLASS()
class WILLBEAOS_API AItemStore : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> StoreWidgetClass;
	UPROPERTY(EditAnywhere)
	UUserWidget* StoreWidget;

public:
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere)
	USphereComponent* SPCollision;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	bool IsShowStore = false;
	
public:	
	AItemStore();

	void Shop();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};