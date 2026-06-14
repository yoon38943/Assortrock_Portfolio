#pragma once

#include "CoreMinimal.h"
#include "Character/WCharacterBase.h"
#include "Character/UI/PlayerHPInfoBar.h"
#include "Components/ActorComponent.h"
#include "Interface/VisibleSightInterface.h"
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

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	IVisibleSightInterface* HpInterface;

	UPROPERTY()
	AWCharacterBase* OwnerCharacter;
	
	UPROPERTY()
	UWidgetComponent* HealthBarWidget;

	FTimerHandle visibleTimerHandle;
	
	void CheckVisibility();

	UPROPERTY()
	UPlayerHPInfoBar* CachedHealthBar;

	void SetWidgetScaleByDistance();

public:	
	UPROPERTY(EditAnywhere, category = "Distance")
	float SightRadius = 3500.f;
	
	bool bIsVisibleEnemy = false;
};
