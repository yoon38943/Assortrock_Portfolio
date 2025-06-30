#pragma once

#include "CoreMinimal.h"
#include "Character/AOSCharacter.h"
#include "WMinionsCharacterBase.generated.h"

#define KILLGOLD 30

class AGamePlayerController;
class AWCharacterBase;
class UAnimMontage;
class UCombatComponent;
class UWidgetComponent;
class UProgressBar;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDetectedSignature, AActor*, DetectedActor);

UCLASS()
class WILLBEAOS_API AWMinionsCharacterBase : public AAOSCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCombatComponent* CombatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* PerceptionCollision;

public:
	AWMinionsCharacterBase();

public:
	// ----- HP 위젯 조절 함수 -----
	bool bLastVisibleState = true;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	float MaxVisibleDistance = 5000.f;		// 최대 가시 거리

	UPROPERTY(EditAnywhere, Category = "UI")
	float MinWidgetScale = 0.2f;

	UPROPERTY(EditAnywhere, Category = "UI")
	float MaxWidgetScale = 1.f;
	
	AWCharacterBase* PlayerChar;
	AGamePlayerController* PlayerController;

	UFUNCTION()
	void FindPlayerPC();
	void FindPlayerPawn();

	// 타겟 인식 관련 로직
	FTimerHandle CheckDistanceTimerHandle;

	float VisibleWidgetDistance = 1200.f;

	TSet<AActor*> CurrentObserveObjects;

	void CheckDistanceToTarget();

	UFUNCTION(BlueprintNativeEvent)
	void CanAttackToTarget(AActor* WObject);
	UFUNCTION(BlueprintNativeEvent)
	void DetachToTarget(AActor* WObject);

public://트랙 관련
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Track")
	int32 TrackNum;

	UFUNCTION(BlueprintNativeEvent, Category = "Track")
	void SetTrackPoint();//트랙 정하는 이벤트
	void SetTrackPoint_Implementation(){}
	
public:	//골드 관련
	virtual void SetGoldReward(int32 NewGold){GoldReward = NewGold;}
	
public:	//타격 관련
	AController* LastHitBy;		// 마지막 타격 주체 저장
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* MinionAttackMontage;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleApplyPointDamage(FHitResult LastHit);//포인트 데미지를 줄시 델리게이트로 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	float CharacterDamage;	//데미지
	UFUNCTION(NetMulticast,BlueprintCallable, Reliable)
	void NM_Minion_Attack();

public:	//체력관련
	UFUNCTION(NetMulticast, Reliable)
	void SetHpPercentage(float Health, float MaxHealth);
	UFUNCTION(Server, Reliable)
	void S_SetHpPercentage(float Health, float MaxHealth);
	UFUNCTION(Server, Reliable)
	void S_SetHPbarColor();
	UFUNCTION(NetMulticast, Reliable)
	void SetHPbarColor(FLinearColor HealthBarColor);
	void RetrySetHPbarColor(FLinearColor HealthBarColor);

public: //죽을 때
	UPROPERTY(BlueprintReadWrite, Category = Dead)
	UAnimMontage* DeadAnimMontage;
	UFUNCTION()
	void Dead();
	UFUNCTION(NetMulticast, Reliable)
	void NM_BeingDead();

	// 게임 엔딩
	void HandleGameEnd();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Destroyed() override;
	
	virtual void Tick(float DeltaTime) override;
};
