#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Game/WGameInstance.h"
#include "GameFramework/Pawn.h"
#include "Wolf.generated.h"

UCLASS()
class WILLBEAOS_API AWolf : public ACharacter
{
	GENERATED_BODY()

public:
	AWolf();

	void LaunchWolf(AActor* InInstigator);

	E_TeamID TeamID;

protected:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* TrailEffect;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DashMontage;

	float DashSpeed = 1300.f;
	float DashEndDistance = 1200.f;

	float TotalMoveDistance;
	
	float ExplosionRadius = 150.f;

	float DashDamage = 30.f;

	float ExplosionDamage = 50.f;

	UPROPERTY()
	AActor* WolfInstigator; 

	TArray<TWeakObjectPtr<AActor>> HitActors;

	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTag EventDamageTag;

	FVector PrevLocation;

	void DashToForward(float DeltaTime);

	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplodeParticle;

	void Explosion(const FVector& ImpactLocation);

	UFUNCTION(NetMulticast, Reliable)
	void Explode_Particle_Multicast(const FVector& ImpactLocation);

	UFUNCTION(NetMulticast, Reliable)
	void Disappear();

	bool bIsDisappear = false;
	
	void CheckPathHit();
};
