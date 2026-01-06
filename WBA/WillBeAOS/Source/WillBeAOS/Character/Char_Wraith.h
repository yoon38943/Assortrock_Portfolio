#pragma once

#include "CoreMinimal.h"
#include "Character/WCharacterBase.h"
#include "PersistentGame/PlayGameState.h"
#include "Shinbi/Skill/SkillDataTable.h"
#include "Char_Wraith.generated.h"

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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
	// 스킬 관련 함수
	virtual void ActivateSkill_Implementation(ESkillSlot SkillSlot) override;
	
	virtual void Handle_UseSkillButton(ESkillSlot Skillslot) override;	// 스킬 input switch 함수
	
	// Q스킬
	UAnimMontage* SkillQMontage;
	void SkillQ_Shot();

	UFUNCTION(NetMulticast, Reliable)
	void NM_SpawnProjectile(const FVector& SocketLocation, const FVector& HitLocation, const FRotator& ProjectileRot, bool SkillBullet);

	UPROPERTY(BlueprintReadOnly)
	bool bIsZoomIn = false;
	UPROPERTY(Replicated)
	bool bEnableQSkill = true;
	FSkillDataTable* QSkill;

	FTimerHandle S_SkillQTimer;
	float QSkillCooldownTime;

	FTimerHandle ZoomTimer;

	UFUNCTION(BlueprintNativeEvent)
	void ClickQButton();
	void ZoomInScope();
	void ZoomOutScope();
	UFUNCTION(Server, Reliable)
	void SetZoomInBool(bool bZoomIn);
	void UpdateZoom();
	UFUNCTION(Server, Reliable)
	void ChangeCharacterSpeed(float Speed);
	UFUNCTION(Client, Reliable)
	void C_ChangeCharacterSpeed(float Speed);
	void SkillQAttack();
	UFUNCTION(Server, Reliable)
	void S_SkillQAttack();
	UFUNCTION(BlueprintNativeEvent)
	void BP_EnhancedAttack();
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void EnhancedAttack(const FVector& Start, const FVector& Direction, const FVector& SocketLocation);

	//  스킬 사용시 애니메이션 실행 함수
	UPROPERTY(ReplicatedUsing = "OnRep_OnQSkillFiring")
	bool bOnQSkillFiring;
	UFUNCTION()
	void OnRep_OnQSkillFiring();

	void QSKillEffect();

	virtual void CallRecall() override;
};
