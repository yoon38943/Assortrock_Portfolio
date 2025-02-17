#pragma once

#include "CoreMinimal.h"
#include "Destructible.h"
#include "GameFramework/Character.h"
#include "WMinionsCharacterBase.generated.h"

class UAnimMontage;
class UCombatComponent;
class UWidgetComponent;
class UProgressBar;

UCLASS()
class WILLBEAOS_API AWMinionsCharacterBase : public ACharacter, public IDestructible
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCombatComponent* CombatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;
	
public:
	AWMinionsCharacterBase();
	
	UPROPERTY(BlueprintReadWrite, Category = Combo)
	TArray<UAnimMontage*> AttackMontages = {};

	UPROPERTY(BlueprintReadWrite, Category = Dead)
	UAnimMontage* DeadAnimMontage;
	
	AController* LastHitBy;		// 마지막 타격 주체 저장

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	int32 GoldReward = 30;

	virtual int32 GetGoldReward() const override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleApplyPointDamage(FHitResult LastHit);//포인트 데미지를 줄시 델리게이트로 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	float CharacterDamage;	//데미지
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(NetMulticast, Reliable)
	void SetHpPercentage(float Health, float MaxHealth);
	UFUNCTION(Server, Reliable)
	void S_SetHpPercentage(float Health, float MaxHealth);
	
	UFUNCTION()
	void Dead();
	UFUNCTION(NetMulticast, Reliable)
	void NM_BeingDead();	// Multicast로 Client들에게 전달
	
	UFUNCTION(NetMulticast,BlueprintCallable, Reliable)
	void NM_Minion_Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* MinionAttackMontage;
};
