#include "WCharacterBase.h"

#include "AOSActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WPlayerController.h"
#include "WPlayerState.h"
#include "Components/SceneComponent.h"
#include "Game/WGameMode.h"
#include "Net/UnrealNetwork.h"


class AAOSActor;

AWCharacterBase::AWCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	this->bUseControllerRotationPitch = false;
	this->bUseControllerRotationRoll = false;
	this->bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraBoom->bUsePawnControlRotation = true;

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetCombatEnable(false);
}


void AWCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	//HandleApplyPointDamage 멀티델리게이트 바인딩
	CombatComp->DelegatePointDamage.AddUObject(this, &ThisClass::HandleApplyPointDamage);

	AWPlayerController* PC = Cast<AWPlayerController>(GetController());
	if (PC)
	{
		FVector StartLocation = GetActorLocation();  // 현재 위치
	
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(FVector(0, 0, 100) - StartLocation).Rotator();
    
		PC->SetControlRotation(LookAtRotation);
	}
}

void AWCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterDamage);
}

void AWCharacterBase::Tick(float DeltaTime)	
{
	Super::Tick(DeltaTime);

	AWPlayerState* PS = Cast<AWPlayerState>(GetPlayerState());
	if (PS)
	{
		GetCharacterMovement()->MaxWalkSpeed = PS->CSpeed;
	}

	UpdateAcceleration();
}

void AWCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// IMC 세팅
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(IMC_Asset, 0);
		}
	}

	// InputAction 붙이기
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AWCharacterBase::Look);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AWCharacterBase::Move);
		EnhancedInputComponent->BindAction(IA_Behavior, ETriggerEvent::Started, this, &AWCharacterBase::Attack);
		EnhancedInputComponent->BindAction(IA_SkillR, ETriggerEvent::Started, this, &AWCharacterBase::SkillR);
		EnhancedInputComponent->BindAction(IA_Recall, ETriggerEvent::Started, this, &ThisClass::CallRecall);
	}
}

void AWCharacterBase::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWCharacterBase::Move(const FInputActionValue& Value)
{
	AWPlayerController* PC = Cast<AWPlayerController>(GetController());
	if (PC && PC->IsRecalling)
	{
		PC->CancelRecall();
	}
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRatator(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRatator).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRatator).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWCharacterBase::Attack()
{
	if (IsDead == true) return;
	
	AWPlayerController* PC = Cast<AWPlayerController>(GetController());
	if (PC && PC->IsRecalling)
	{
		PC->CancelRecall();
	}
	
	if (HasAuthority())
	{
		Behavior();
	}
	else
	{
		S_Behavior();
	}
}

void AWCharacterBase::Behavior()
{
	CombatComp->SetCollisionMesh(GetMesh());
	if (CombatComp != nullptr)
	{
		//공격중이 아닐시
		if ((CombatComp->IsCombatEnable() == false))
		{
			//공격중 활성화
			CombatComp->SetCombatEnable(true);
			DSkillLCooldown.ExecuteIfBound();
			//콤보 로직
			if ((CombatComp->GetAttackCount()) < AttackMontages.Num())
			{
				NM_Behavior(CombatComp->GetAttackCount());
				CombatComp->AddAttackCount(1);
				if (CombatComp->GetAttackCount() >= AttackMontages.Num())
				{
					CombatComp->ResetCombo();
				}
			}
		}
	}
}

void AWCharacterBase::S_Behavior_Implementation()
{
	Behavior();
}

void AWCharacterBase::NM_Behavior_Implementation(int32 Combo)
{
	ACharacter::PlayAnimMontage(AttackMontages[Combo]);
}

void AWCharacterBase::SkillR(const FInputActionValue& Value)
{
	if (CombatComp != nullptr)
	{
		//�������� �ƴҽ�
		if ((CombatComp->IsCombatEnable() == false))
		{
			if (SkillREnable == false) 
			{
				//������ Ȱ��ȭ
				CombatComp->SetCombatEnable(true);
				//��ųR���
				SkillREnable = true;

				DSkillRCooldown.ExecuteIfBound();
				//��Ÿ�� ����
				if ((CombatComp->GetAttackCount()) < AttackMontages.Num())
				{
					ACharacter::PlayAnimMontage(SkillRMontage);
				}
			}
		}
	}
}

void AWCharacterBase::UpdateAcceleration()
{
	float CurrentSpeed = GetCharacterMovement()->Velocity.Size();
	float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// 현재 속도에 비례해서 가속도를 조정 (최대 속도가 높을수록 가속도 증가)
	float NewAcceleration = FMath::Lerp(2048.0f, 5000.0f, CurrentSpeed / MaxSpeed);
	GetCharacterMovement()->MaxAcceleration = NewAcceleration;
}

void AWCharacterBase::CallRecall()
{
	AWPlayerController* PC = Cast<AWPlayerController>(GetController());
	if (PC)
	{
		PC->StartRecall();
	}
}

void AWCharacterBase::ServerPlayMontage_Implementation(UAnimMontage* Montage)
{
	MultiPlayMontage(Montage);
}

void AWCharacterBase::MultiPlayMontage_Implementation(UAnimMontage* Montage)
{
	if (Montage)
	{
		PlayAnimMontage(Montage);
	}
}

void AWCharacterBase::SpawnHitEffect_Implementation(FVector HitLocation)
{
}

void AWCharacterBase::NM_SpawnHitEffect_Implementation(FVector HitLocation)
{
	SpawnHitEffect(HitLocation);
}

void AWCharacterBase::BeingDead()
{
	IsDead = true;
	
	//죽음 메세지 출력
	auto Message = FString::Printf(TEXT("Dead"));
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, Message);

	AWPlayerController* PC = Cast<AWPlayerController>(GetController());
	
	C_BeingDead(PC);	// 클라에서 실행하는 것
	S_BeingDead(PC, this);	// 서버에서 실행하는 것
}

void AWCharacterBase::S_BeingDead_Implementation(AWPlayerController* PC, APawn* Player)
{
	bIsDead = true;
	
	//캐릭터 리스폰
	AWGameState* GameState = Cast<AWGameState>(GetWorld()->GetGameState());
	AWGameMode* GameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
	if (PC && GameState && GameMode && HasAuthority())
	{
		FTimerHandle RespawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle,
			[Player, PC, GameMode]()
			{
				GameMode->RespawnPlayer(Player, PC);
			}, GameState->RespawnTime, false);
	}
	
	NM_BeingDead();
}

void AWCharacterBase::NM_BeingDead_Implementation()
{
	//무브먼트, 콜리전 없애고 몽타주 출력
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PlayAnimMontage(DeadAnimMontage);
}

void AWCharacterBase::C_BeingDead_Implementation(AWPlayerController* PC)
{
	// 클라에서 리스폰 위젯 출력
	if (PC != nullptr)
	{
		//리스폰 실행
		PC->ShowRespawnWidget();
	}

	//죽으면 카메라 움직임에 메쉬 따라 움직이지 않게 하기
	this->bUseControllerRotationYaw = false;
}

//포인트 데미지 주는 함수
void AWCharacterBase::HandleApplyPointDamage(FHitResult LastHit)
{
	if (HasAuthority())
	{
		// ----- 같은팀 캐릭터, 미니언 타격 무효 -----
		AAOSCharacter* HitCharacter = Cast<AAOSCharacter>(LastHit.GetActor());
		if (HitCharacter)
		{
			if (this->TeamID == HitCharacter->TeamID) return;
		}
		// ----- 같은팀 타워, 넥서스 타격 무효 -----
		AAOSActor* HitObject = Cast<AAOSActor>(LastHit.GetActor());
		if (HitObject)
		{
			if (this->TeamID == HitObject->TeamID) return;
		}

		NM_SpawnHitEffect(LastHit.Location);
		
		AWPlayerController* PC = Cast<AWPlayerController>(GetController());
		if (PC)
		{
			AWPlayerState* PState = PC->GetPlayerState<AWPlayerState>();
			if (PState)
			{
				CharacterDamage = PState->CPower;
			}
		}
		
		UGameplayStatics::ApplyPointDamage(
			LastHit.GetActor(),
			CharacterDamage,
			GetOwner()->GetActorForwardVector(),
			LastHit,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
	}
}

// 데미지 받는 함수
float AWCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		AWPlayerState* PS = Cast<AWPlayerState>(GetPlayerState());
		if (PS)
		{
			PS->Server_ApplyDamage(DamageAmount);
		}
	}

	//ServerPlayMontage(HitAnimMontage);
	return DamageAmount;
}