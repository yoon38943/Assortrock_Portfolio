#pragma once

#include "CoreMinimal.h"
#include "Character/WCharacterBase.h"
#include "Shinbi/Skill/SkillDataTable.h"
#include "Char_Wraith.generated.h"

class APlayGameState;

UCLASS()
class WILLBEAOS_API AChar_Wraith : public AWCharacterBase
{
	GENERATED_BODY()

	AChar_Wraith();

protected:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ScopeAttackCameraPoint;

	APlayGameState* GS;

	UPROPERTY(EditDefaultsOnly, Category = "HitParticle")
	UParticleSystem* HitParticle;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> Projectile_Normal;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> Projectile_Enhanced;
	UPROPERTY(EditAnywhere)
	UAnimMontage* ZoomInMontage;

public:
	AActor* LastTarget;
	// 타겟 락온 관련
	AActor* CheckTargettingInCenter();

	UPROPERTY(Replicated)
	float TargettingTraceLength = 1500.f;

	// 공격
	bool CanAttack = true;

	virtual void Attack() override;
	
	virtual void Behavior() override;

	UFUNCTION(BlueprintNativeEvent)
	void AttackFire();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void WraithAttack(const FVector& Start, const FVector& Direction, const FVector& SocketLocation);

	UFUNCTION(NetMulticast, Reliable)
	void NM_HitEffect(const FVector& HitLocation);

public:
	// Q스킬 사용
	virtual void SkillQ() override;

	bool bIsZoomIn;
	FSkillDataTable* QSkill;

	FTimerHandle ZoomTimer;
	
	void ZoomInScope();
	void ZoomOutScope();
	void UpdateZoom();
	void SkillQAttack();
	UFUNCTION(Server, Reliable)
	void S_SkillQAttack();
	UFUNCTION(BlueprintNativeEvent)
	void BP_EnhancedAttack();
	void EnhancedAttack(const FVector& Start, const FVector& Direction, const FVector& SocketLocation);
	UFUNCTION(NetMulticast, Reliable)
	void NM_SkillPlayMontage(UAnimMontage* SkillMontage);
};
