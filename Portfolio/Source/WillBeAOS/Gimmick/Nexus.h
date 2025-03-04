#pragma once

#include "CoreMinimal.h"
#include "Character/AOSActor.h"
#include "GameFramework/Actor.h"
#include "Nexus.generated.h"

UCLASS()
class WILLBEAOS_API ANexus : public AAOSActor
{
	
	GENERATED_BODY()

	int NexusHP;
	
public:	//타격 관련
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	
public: //채력 관련
	float GetNexusHPPercent();
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneRootComponent;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollisionComponet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* NexusMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCombatComponent* CombatComp;

public:	
	ANexus();

};
