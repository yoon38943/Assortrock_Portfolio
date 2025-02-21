#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "WEnumFile.h"
#include "GameFramework/Actor.h"
#include "Iris/ReplicationSystem/ReplicationSystemTypes.h"
#include "Tower.generated.h"

class USceneComponent;
class UCapsuleComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class WILLBEAOS_API ATower : public AActor, public IDestructible
{

	GENERATED_BODY()
	
public:
	
	ATower();
	
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	
	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	class AWGameState* AWGS;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Team")
	E_TeamID TowerTeamID = E_TeamID::Neutral;

	virtual int32 GetGoldReward() const override;
	int32 GoldReward = 50;

	AController* LastHitBy;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void SetHpPercentage(float Health, float MaxHealth);
	UFUNCTION(Server, Reliable)
	void S_SetHpPercentage(float Health, float MaxHealth);
	
public:	
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleCollisionComponet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditAnywhere)
	USphereComponent* OverlapTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AttackStartPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* DamagedNiagara;
	UPROPERTY(BlueprintReadWrite)
	class UStaticMesh* DamagedStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* DamageParticle;
	
	bool IsParticleSpawned = false;

public:
	UPROPERTY(BlueprintReadWrite, Category = SpawnActor)
	TSubclassOf<AActor> SpawnActors;
	UPROPERTY(BlueprintReadOnly, Category = SpawnActor)
	AActor* TargetOfActors;
	FVector HitLocation;
	FVector HitNormal;
	FName BoneName;
	FHitResult OutHit;
	// 오버랩된 액터들의 배열 ( 공격 대상들 )
	UPROPERTY(BlueprintReadWrite, Category = SpawnActor)
	TArray<AActor*> OverlappingActors = {};
	ETraceTypeQuery TraceChannel;
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> OutHits;

private:
	UFUNCTION(BlueprintCallable)
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	
	float Delta;
	
	void spawn();
};
