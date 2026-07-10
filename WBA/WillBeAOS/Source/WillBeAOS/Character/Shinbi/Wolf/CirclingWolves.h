#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CirclingWolves.generated.h"

UCLASS()
class WILLBEAOS_API ACirclingWolves : public ACharacter
{
	GENERATED_BODY()

public:
	ACirclingWolves();

	void InitWolves(AActor* InOwner, const float InStartAngle, const float InCircleRadius, const float InLifeTime = 5.f);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* TrailEffect;

private:	
	float CircleRadius = 350.f;
	
	float TurnSpeed = 220.f;

	UPROPERTY()
	AActor* WolvesOwner;

	float CurrentAngle;

	void UpdateCirclingPosition(float DeltaTime);

	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplodeParticle;
};
