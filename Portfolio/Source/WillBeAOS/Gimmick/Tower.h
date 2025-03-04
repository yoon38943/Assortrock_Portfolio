#pragma once

#include "CoreMinimal.h"
#include "WEnumFile.h"
#include "Character/AOSActor.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

class USceneComponent;
class UCapsuleComponent;
class USphereComponent;
class UStaticMeshComponent;

#define GOLDAMOUNT 50
UCLASS()
class WILLBEAOS_API ATower : public AAOSActor
{

	GENERATED_BODY()
	
public:
	ATower();
	
	UPROPERTY(BlueprintReadWrite, Category = "GameState")
	class AWGameState* AWGS;
	
protected://체력관련
	
	UFUNCTION(NetMulticast, Reliable)
	void SetHpPercentage(float Health, float MaxHealth);
	UFUNCTION(Server, Reliable)
	void S_SetHpPercentage(float Health, float MaxHealth);
public:
	UFUNCTION(Server, Reliable)
	void S_SetHPbarColor();
	UFUNCTION(NetMulticast, Reliable)
	void SetHPbarColor(FLinearColor HealthBarColor);

	UFUNCTION(Server, Reliable)
	void S_SetDamaged();
	UFUNCTION(NetMulticast, Reliable)
	void NM_SetDamaged();
	
public://스폰
	float Delta;
	
	void spawn();
	
public:	
	UPROPERTY(EditAnywhere)
	USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleCollisionComponet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* OverlapTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* AttackStartPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComp;
	UPROPERTY(BlueprintReadWrite)
	class UStaticMesh* DamagedStaticMesh;
	UFUNCTION(BlueprintNativeEvent)
	void DamagedParticle();
	
	bool IsParticleSpawned = false;

public://타격 관련
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
	
	AController* LastHitBy;

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	
public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION(BlueprintCallable)
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	virtual void BeginPlay() override;
	
};
