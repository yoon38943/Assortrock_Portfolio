#pragma once

#include "CoreMinimal.h"
#include "AOSCharacter.h"
#include "WDelegateDefine.h"
#include "WEnumFile.h"
#include "WCharacterBase.generated.h"

struct FInputActionValue;
class UInputAction;
class UAnimMontage;
class UWidgetComponent;

UCLASS()
class WILLBEAOS_API AWCharacterBase : public AAOSCharacter
{

	GENERATED_BODY()

	//컴포넌트
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"));
	class UCombatComponent* CombatComp;
	
public:
	UPROPERTY(Replicated)
	E_TeamID CharacterTeam;
	
	// HP Widget 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* HPInfoBarComponent;

	UPROPERTY(EditAnywhere, Category = "Color")
	FLinearColor BlueTeamHPColor;
	UPROPERTY(EditAnywhere, Category = "Color")
	FLinearColor RedTeamHPColor;
	UPROPERTY(EditAnywhere, Category = "Color")
	FLinearColor SelfHPColor;
	
	FLinearColor HPInfoBarColor;

	FTimerHandle HealingTimerHandle;
	
	virtual void SetHPInfoBarColor();
	
	virtual void SetHPPercentage();

	virtual void ShowNickName();

	// 플레이어 거리 계산
	TArray<APlayerState*> AllActors;
	
	FTimerHandle CheckDistanceHandle;
	
	void CheckAllPlayerDistance();

	float MaxVisibleDistance = 3500.f;
	
public:
	AWCharacterBase();

private:
	//입력 에셋
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* IMC_Asset;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Jump;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Behavior;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_SkillR;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* IA_Recall;

public:
	UPROPERTY(BlueprintReadonly)
	bool IsDead = false;
	
	UPROPERTY(BlueprintReadWrite, Category = "Health")
	UAnimMontage* DeadAnimMontage;	//죽을???�일 몽�?�?
	UPROPERTY(BlueprintReadWrite, Category = "Health")
	UAnimMontage* HitAnimMontage;	//?�격???�일 몽�?�?
	UPROPERTY(BlueprintReadWrite, Category = Combo)
	TArray<UAnimMontage*> AttackMontages = {};	//�޺��� ���� �ִԸ�Ÿ�� �迭
	UPROPERTY(BlueprintReadWrite, Category = Combo)
	UAnimMontage* SkillRMontage;//R��ų�� ���� ��Ÿ��

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SkillR(const FInputActionValue& Value);
	void UpdateAcceleration();

	// ---- 귀환 관련 함수 ----
	void CallRecall();

	UPROPERTY(EditAnywhere, Category = "Recall")
	UAnimMontage* StartRecallMontage;
	UPROPERTY(EditAnywhere, Category = "Recall")
	UAnimMontage* CompleteRecallMontage;

	UFUNCTION(Server, Reliable)
	void ServerPlayMontage(UAnimMontage* Montage);
	UFUNCTION(NetMulticast, Reliable)
	void MultiPlayMontage(UAnimMontage* Montage);
	UFUNCTION(NetMulticast, Reliable)
	void NM_StopPlayMontage();
	
	// ---- Attack 관련 함수 ----
	void Attack();
	virtual void Behavior();
	UFUNCTION(Server, Reliable)
	void S_Behavior();
	UFUNCTION(NetMulticast, Reliable)
	void NM_Behavior(int32 Combo);

	// ----- Hit 이벤트 -----
	UFUNCTION(BlueprintNativeEvent)
	void SpawnHitEffect(FVector HitLocation);
	UFUNCTION(NetMulticast, Reliable)
	void NM_SpawnHitEffect(FVector HitLocation);

	// ---- Dead 관련 함수 -----
	UFUNCTION(Server, Reliable)
	void S_BeingDead(class AWPlayerController* PC, APawn* Player);
	// 일반 죽는 함수(죽는 클라이언트 본인만 실행되도록)
	void BeingDead();
	// 클라이언트
	UFUNCTION(Client, Reliable)
	void C_BeingDead(AWPlayerController* PC);


	// 델리게이트 함수
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = Dead)
	void NM_BeingDead();//죽을 때 델리게이트로 호출 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleApplyPointDamage(FHitResult LastHit);//?�인???��?지�?줄시 ?�리게이?�로 ?�출???�수
	UFUNCTION(BlueprintCallable, Category = "Combat")//TakeDamage ?�수 ?�버?�이??
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//게임 엔딩
	void HandleGameEnd();
	
	//?�리게이???�의
	FDS_SkillLCooldown DSkillLCooldown;
	FDS_SkillLCooldown DSkillRCooldown;


private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Combat")
	float CharacterDamage;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(BlueprintReadWrite, Category = "Skill")//���� ����
	bool SkillREnable;	
};