#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WDelegateDefine.h"
#include "CombatComponent.generated.h"

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WILLBEAOS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, Category = "Collision")
	FName StartSocket;
	UPROPERTY(EditAnywhere, Category = "Collision")
	FName EndSocket;
	UPROPERTY(VisibleDefaultsOnly)
	bool IsCollisionEnabled = false;
	UPROPERTY(VisibleDefaultsOnly)
	UPrimitiveComponent* CollisionMeshComponent;
	UPROPERTY(EditAnywhere, Category = "Collision")
	float Radius;
	UPROPERTY(EditAnywhere, Category = "Collision")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {};
	
	UFUNCTION()
	void SetCollisionMesh(UPrimitiveComponent* PrimComp);
	
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void CollisionTrace(); 
	UFUNCTION(BlueprintCallable, Category = "Collsion")
	void EnableCollision();
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void DisableCollision();
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void ClearHitActor();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 GetAttackCount();//??? ???
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AddAttackCount(int32 Val);//??? ??? ????
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();//??? ????
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsCombatEnable();//?????????? ???
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetCombatEnable(bool Val);//?????? ????
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void HandleTakeDamage(float Damage);//?????? ????
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetIsDead(bool Val);
	UFUNCTION(BlueprintCallable, Category = "Health")
	bool GetIsDead();

	FDelegateSignature DelegateDead;
	FMDS1 DelegatePointDamage;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BluePrintReadOnly, Category = "Combat")
	bool CombatEnable = false;
	
	UPROPERTY(BlueprintReadWrite, Category = Combo)
	int32 AttackCount = 0;
	
	UPROPERTY(EditAnywhere, Category = "Collision")
	TArray<AActor*> AlreadyHitActors = {};

	UPROPERTY(Replicated ,BlueprintReadOnly, Category = "Health")
	float Health = 10;
	UPROPERTY(EditAnywhere, Category = "Health")
	float Max_Health = 100;
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	bool IsDead;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UCombatComponent();
};
