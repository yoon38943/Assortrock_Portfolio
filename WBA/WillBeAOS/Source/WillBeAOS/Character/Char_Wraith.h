#pragma once

#include "CoreMinimal.h"
#include "Character/WCharacterBase.h"
#include "Char_Wraith.generated.h"

class APlayGameState;

UCLASS()
class WILLBEAOS_API AChar_Wraith : public AWCharacterBase
{
	GENERATED_BODY()

	APlayGameState* GS;

	UPROPERTY(EditDefaultsOnly, Category = "HitParticle")
	UParticleSystem* HitParticle;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> Projectile_Normal;

	UFUNCTION(NetMulticast, reliable)
	void NM_HitParticle(FVector HitLocation);

public:
	AActor* LastTarget;
	// 타겟 락온 관련
	AActor* CheckTargettingInCenter();

	// 공격
	bool CanAttack = true;
	
	virtual void Behavior() override;

	UFUNCTION()
	void AttackFire();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void WraithAttack(const FVector& Start, const FVector& End);
};
