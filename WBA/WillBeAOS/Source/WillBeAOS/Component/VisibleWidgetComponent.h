#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VisibleWidgetComponent.generated.h"


class UWidgetComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WILLBEAOS_API UVisibleWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVisibleWidgetComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* playerWidget;

	FTimerHandle visibleTimerHandle;
	
	void CheckVisibility();

public:	
	UPROPERTY(EditAnywhere)
	float sightRadius = 4000.f;
	
	bool bIsVisibleEnemy = false;
};
