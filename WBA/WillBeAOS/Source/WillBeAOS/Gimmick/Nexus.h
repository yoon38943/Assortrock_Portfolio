#pragma once

#include "CoreMinimal.h"
#include "Character/AOSActor.h"
#include "GameFramework/Actor.h"
#include "Nexus.generated.h"

UCLASS()
class WILLBEAOS_API ANexus : public AAOSActor
{
	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "GameEnd")
	UParticleSystem* DestroyParticle;

	UPROPERTY(EditAnywhere, Category = "GameEnd")
	TSubclassOf<AActor> EndingCameraClass;
	
public:	//타격 관련
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	
public: //채력 관련
	float GetNexusHPPercent();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneRootComponent;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollisionComponet1;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollisionComponet2;
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollisionComponet3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* NexusMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCombatComponent* CombatComp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* EndingCameraComponent;

public:	
	ANexus();

	void DestroyNexus();

	UFUNCTION(NetMulticast, reliable)
	void NM_DestroyNexus();

};
