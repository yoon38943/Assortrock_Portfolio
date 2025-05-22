#pragma once

#include "CoreMinimal.h"
#include "Character/AOSCharacter.h"
#include "Function/WEnemyDetectorComponent.h"
#include "WMinionsCharacterBase.generated.h"

#define KILLGOLD 30

class AWPlayerController;
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
	AWPlayerController* PlayerController;

	UFUNCTION()
	void FindPlayerPC();
	void FindPlayerPawn();

public://트랙 관련
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadOnly, Category = "Track")
	int32 TrackNum;

	UFUNCTION(BlueprintNativeEvent, Category = "Track")
	void SetTrackPoint();//트랙 정하는 이벤트
	void SetTrackPoint_Implementation(){}

	// 적 탐지
	UPROPERTY(BlueprintAssignable, Category="Detection")
	FOnEnemyDetectedSignature OnEnemyDetected;
	
	UPROPERTY(VisibleAnywhere, Category = "Perception")
	UWEnemyDetectorComponent* EnemyDetector;

	UFUNCTION(BlueprintCallable)
	void HandleEnemyDetected(AActor* Enemy);
	
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

protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};
