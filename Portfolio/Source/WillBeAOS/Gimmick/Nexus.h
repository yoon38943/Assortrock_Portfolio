#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "GameFramework/Actor.h"
#include "Nexus.generated.h"

UCLASS()
class WILLBEAOS_API ANexus : public AActor
{
	GENERATED_BODY()
	
public:	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	float GetNexusHPPercent();

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Team")
	E_TeamID NexusTeamID = E_TeamID::Neutral;
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneRootComponent;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollisionComponet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* NexusMeshComponent;
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComp;

public:	
	ANexus();

};
