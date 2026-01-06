#include "WCharacterBase.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponent.h"
#include "WCharAnimInstance.h"
#include "Component/VisibleWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/SceneComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Gimmick/Tower.h"
#include "Kismet/KismetMathLibrary.h"
#include "Minions/WMinionsCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "PersistentGame/GamePlayerController.h"
#include "PersistentGame/GamePlayerState.h"
#include "PersistentGame/PlayGameMode.h"
#include "PersistentGame/PlayGameState.h"
#include "UI/PlayerHPInfoBar.h"


AWCharacterBase::AWCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetCombatEnable(false);

	HPInfoBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPInfoBar"));
	HPInfoBarComponent->SetupAttachment(GetRootComponent());
	HPInfoBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	HPInfoBarComponent->SetVisibility(false);

	SetReplicateMovement(true);
	bAlwaysRelevant = true;

	SetGoldReward(PLAYERKILLGOLD);

	bUseControllerRotationYaw = false;
	TurningInPlace = E_TurningInPlace::E_NotTurning;

	bReplicates = true;

	SightComp = CreateDefaultSubobject<UVisibleWidgetComponent>(TEXT("SightComponent"));
}


void AWCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
		UE_LOG(LogTemp, Warning, TEXT("AWCharacterBase::BeginPlay()"));
	//HandleApplyPointDamage 멀티델리게이트 바인딩
	CombatComp->DelegatePointDamage.AddUObject(this, &ThisClass::HandleApplyPointDamage);

	APlayGameMode* GM = Cast<APlayGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		// 게임 종료 델리게이트 바인딩 ( 서버에서만 일어남 )
		GM->OnGameEnd.AddUObject(this, &ThisClass::HandleGameEnd);
	}

	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	if (PC)
	{			
		FVector StartLocation = GetActorLocation();  // 현재 위치
	
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(FVector(0, 0, 100) - StartLocation).Rotator();
    
		PC->SetControlRotation(LookAtRotation);
	}

	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		GetCharacterMovement()->MaxWalkSpeed = PS->CSpeed;
	}
	
	if (!HasAuthority())
	{
		SetHPInfoBarColor();
		SetHPPercentage();
		ShowNickName();
	}
	else
	{
		APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->GameManagedActors.AddUnique(this);
		}
	}
}

void AWCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (CheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
	}
}

void AWCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateAcceleration();

	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	if (PC)
	{
		IsRecalling = PC->IsRecalling;
	}

	AimOffset(DeltaTime);

	VisibleOutline();
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
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AWCharacterBase::Move);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &AWCharacterBase::StopMove);
		EnhancedInputComponent->BindAction(IA_Behavior, ETriggerEvent::Started, this, &AWCharacterBase::Attack);
		EnhancedInputComponent->BindAction(IA_SkillQ, ETriggerEvent::Started, this, &AWCharacterBase::Input_QSkill);
		// E 스킬 자리
		// R 스킬 자리
		EnhancedInputComponent->BindAction(IA_Recall, ETriggerEvent::Started, this, &ThisClass::CallRecall);
	}
}

void AWCharacterBase::AimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();

	if (Speed == 0.f)
	{
		if (IsLocallyControlled())
		{
			FRotator AimRot = GetBaseAimRotation();
			FRotator ActorRot = GetActorRotation();

			FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRot, ActorRot);
        
			Yaw = DeltaRot.Yaw;
			Server_SetYaw(Yaw);
		}
		
		bUseControllerRotationYaw = false;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f)
	{
		StartAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		bUseControllerRotationYaw = true;
		Yaw = 0.f;
		TurningInPlace = E_TurningInPlace::E_NotTurning;
	}

	Pitch = GetBaseAimRotation().Pitch;
	if (Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, Pitch);
	}
}

void AWCharacterBase::Server_SetControlRotation_Implementation(FRotator Rotation)
{
	ControllerRotation = Rotation;
}

void AWCharacterBase::Server_SetYaw_Implementation(float YawValue)
{
	Yaw = YawValue;
}

void AWCharacterBase::TurnInPlace(float DeltaTime)
{
	if (Yaw > 90.f && !IsRecalling)
	{
		TurningInPlace = E_TurningInPlace::E_TurningRight;
	}
	else if (Yaw < -90.f && !IsRecalling)
	{
		TurningInPlace = E_TurningInPlace::E_TurningLeft;
	}
	if (TurningInPlace != E_TurningInPlace::E_NotTurning)
	{
		GetCharacterMovement()->RotationRate = FRotator(0.f, 250.f, 0.f);
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
        
		if (FMath::Abs(Yaw) < 5.f)
		{
			TurningInPlace = E_TurningInPlace::E_NotTurning;
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			StartAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AWCharacterBase::SetHPInfoBarColor()
{
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS && PS->PlayerInfo.PlayerTeam != E_TeamID::Neutral)
	{
		UPlayerHPInfoBar* HPInfoBar = Cast<UPlayerHPInfoBar>(HPInfoBarComponent->GetWidget());
		if (HPInfoBar)
		{
			if (IsLocallyControlled())
			{
				HPInfoBarColor = SelfHPColor;
			
				HPInfoBar->PlayerHPBar->SetFillColorAndOpacity(HPInfoBarColor);

				HPInfoBar->InvalidateLayoutAndVolatility();
			}
			else
			{
				if (PS->PlayerInfo.PlayerTeam == E_TeamID::Blue)
				{
					HPInfoBarColor = BlueTeamHPColor; 
				}
				else if (PS->PlayerInfo.PlayerTeam == E_TeamID::Red)
				{
					HPInfoBarColor = RedTeamHPColor;
				}
			
				HPInfoBar->PlayerHPBar->SetFillColorAndOpacity(HPInfoBarColor);

				HPInfoBar->InvalidateLayoutAndVolatility();
			}
		}
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ThisClass::SetHPInfoBarColor, 0.1f, false);
	}
}

void AWCharacterBase::SetHPPercentage()
{
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		if (UPlayerHPInfoBar* HPInfoBar = Cast<UPlayerHPInfoBar>(HPInfoBarComponent->GetWidget()))
		{
			HPInfoBar->PlayerHPBar->SetPercent(PS->GetHPPercentage());
			HPInfoBar->InvalidateLayoutAndVolatility();
		}
		else
		{
			// 위젯이 아직 안 붙었을 경우 딜레이 후 재시도
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
			{
				SetHPPercentage();
			}, 0.1f, false);
		}
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ThisClass::SetHPPercentage, 0.1f, false);
	}
}

void AWCharacterBase::ShowNickName()
{
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		if (PS->PlayerInfo.PlayerNickName == "")
		{
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, this, &ThisClass::ShowNickName, 0.1f, false);
		}
		else
		{
			if (UPlayerHPInfoBar* HPInfoBar = Cast<UPlayerHPInfoBar>(HPInfoBarComponent->GetWidget()))
			{
				HPInfoBar->PlayerNickName->SetText(FText::FromString(PS->PlayerInfo.PlayerNickName));
			}
		}
	}
	else
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, this, &ThisClass::ShowNickName, 0.1f, false);
	}
}

// 시야체크 완료하면 삭제
/*void AWCharacterBase::Server_SetVisibleWidgetDistance()
{
	FVector MyLocation = GetActorLocation();
	float VisibleDistanceSqr = FMath::Square(VisibleWidgetDistance);

	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		for (TWeakObjectPtr<AActor> WeakActor : GS->CachedActors)
		{
			if (WeakActor.IsValid())
			{
				AActor* Actor = WeakActor.Get();
				if (!IsValid(Actor) || Actor == this) continue;

				float DistSqr = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
				bool bShouldShow = DistSqr <= VisibleDistanceSqr;

				SetWidgetVisible(Actor, bShouldShow);
			}
		}
	}
}

void AWCharacterBase::SetWidgetVisible_Implementation(AActor* Actor, bool bIsVisible)
{
	if (AWCharacterBase* Character = Cast<AWCharacterBase>(Actor))
	{
		if (Character->bIsDead == true)
		{
			bIsVisibleEnemy = false;
		}
		
		if (Character->HPInfoBarComponent && Character->HPInfoBarComponent->IsVisible() != bIsVisibleEnemy)
		{
			Character->HPInfoBarComponent->SetVisibility(bIsVisibleEnemy);
		}
	}
	else if (AWMinionsCharacterBase* Minion = Cast<AWMinionsCharacterBase>(Actor))
	{
		if (Minion->bIsDead == true)
		{
			bIsVisible = false;
		}
		
		if (Minion->WidgetComponent && Minion->WidgetComponent->IsVisible() != bIsVisible)
		{
			Minion->WidgetComponent->SetVisibility(bIsVisible);
		}
	}
	else if (ATower* Tower = Cast<ATower>(Actor))
	{
		if (Tower->WidgetComponent && Tower->WidgetComponent->IsVisible() != bIsVisible)
		{
			Tower->WidgetComponent->SetVisibility(bIsVisible);
		}
	}
}*/

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
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	if (PC && PC->IsRecalling)
	{
		PC->Server_CancelRecall();
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

	FRotator ControlRot = GetControlRotation();
	FRotator OnlyYaw = FRotator(0.f, ControlRot.Yaw, 0.f);
	Server_SetControlRotationYaw(OnlyYaw);
}

void AWCharacterBase::StopMove(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
}

void AWCharacterBase::VisibleOutline()
{
	if (!HasAuthority() && IsLocallyControlled())
	{
		// 캐릭터 죽었을 경우 현재 타겟 모두 없애기
		if (bIsDead)
		{
			if (AttackTarget.Num() > 0)
			{
				TArray<AActor*> DeleteActors;
				for (auto& Actor : AttackTarget)
				{
					DeleteActors.Add(Actor);
				}

				for (auto& Actor : DeleteActors)
				{
					UE_LOG(LogTemp, Warning, TEXT("%s"), *Actor->GetName());
					AttackTarget.Remove(Actor);
				}
			}
		}

		// 타겟에 변화가 일어 났을 때
		if (AttackTarget != CurrentTarget)
		{
			// 타겟이 줄었을 때
			if (CurrentTarget.Num() > 0)
			{
				// 타겟이 아니게된 액터 찾아서 외곽선 끄기
				for (auto& Actor : CurrentTarget)
				{
					if (Actor && !AttackTarget.Contains(Actor))
					{
						auto EnemyMesh = Actor->FindComponentByClass<UMeshComponent>();
						if (EnemyMesh)
						{
							EnemyMesh->SetRenderCustomDepth(false);
						}
					}
				}
			}

			// 타겟이 늘었을 때
			if (AttackTarget.Num() > 0)
			{
				// 새로 타겟이 된 액터 찾아서 외곽선 표시하기
				for (auto& Actor : AttackTarget)
				{
					if (Actor && !CurrentTarget.Contains(Actor))
					{
						auto EnemyMesh = Actor->FindComponentByClass<UMeshComponent>();
						if (EnemyMesh)
						{
							EnemyMesh->SetRenderCustomDepth(true);
						}
					}
				}
			}

			CurrentTarget = AttackTarget;	// 외곽선 표시처리 후 배열 동일하게 처리
		}
	}
}

void AWCharacterBase::ChangeSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AWCharacterBase::NM_StopPlayMontage_Implementation()
{
	StopAnimMontage();
}

void AWCharacterBase::Attack()
{
	if (bIsDead == true) return;
	
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	if (PC && PC->IsRecalling)
	{
		PC->Server_CancelRecall();
	}
	
	if (!HasAuthority())
	{
		S_Behavior();
	}
}

void AWCharacterBase::Behavior()
{
	EnterCombat();
	
	CombatComp->SetCollisionMesh(GetMesh());
	if (CombatComp != nullptr)
	{
		//공격중이 아닐시
		if (CombatComp->IsCombatEnable() == false)
		{
			//공격중 활성화
			CombatComp->SetCombatEnable(true);
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

void AWCharacterBase::EnterCombat()
{
	IsCombat = true;
	
	GetWorld()->GetTimerManager().ClearTimer(CombatTimer);
	GetWorld()->GetTimerManager().SetTimer(CombatTimer, this, &ThisClass::ExitCombat, 8.f, false);
}

void AWCharacterBase::ExitCombat()
{
	IsCombat = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		UWCharAnimInstance* Anim = Cast<UWCharAnimInstance>(AnimInstance);
		if (Anim)
		{
			Anim->IsInCombat = false;
		}
	}
}

void AWCharacterBase::Input_QSkill(const FInputActionValue& Value)
{
	Handle_UseSkillButton(ESkillSlot::Q);
}

void AWCharacterBase::Input_ESkill(const FInputActionValue& Value)
{
	Handle_UseSkillButton(ESkillSlot::E);
}

void AWCharacterBase::Input_RSkill(const FInputActionValue& Value)
{
	Handle_UseSkillButton(ESkillSlot::R);
}

void AWCharacterBase::Handle_UseSkillButton(ESkillSlot Skillslot)
{
	// 자식 함수에서 정의
}

void AWCharacterBase::S_Behavior_Implementation()
{
	Behavior();
}

void AWCharacterBase::NM_Behavior_Implementation(int32 Combo)
{
	ACharacter::PlayAnimMontage(AttackMontages[Combo]);
}

void AWCharacterBase::UpdateAcceleration()
{
	float CurrentSpeed = GetCharacterMovement()->Velocity.Size();
	float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// 현재 속도에 비례해서 가속도를 조정 (최대 속도가 높을수록 가속도 증가)
	float NewAcceleration = FMath::Lerp(2048.0f, 5000.0f, CurrentSpeed / MaxSpeed);
	GetCharacterMovement()->MaxAcceleration = NewAcceleration;
}

void AWCharacterBase::Server_SetControlRotationYaw_Implementation(FRotator YawRotation)
{
	SetActorRotation(YawRotation);	// 서버에서 캐릭터 회전값 동기화
}

bool AWCharacterBase::Server_SetControlRotationYaw_Validate(FRotator YawRotation)
{
	return true;
}

void AWCharacterBase::CallRecall()
{
	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
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

void AWCharacterBase::ActivateSkill_Implementation(ESkillSlot SkillSlot)
{
	// 자식 캐릭터 클래스에서 채움
}

void AWCharacterBase::ExecuteSkill(ESkillSlot SkillSlot)
{
	// 자식 캐릭터 클래스에서 채움
}

void AWCharacterBase::BeingDead()
{
	bIsDead = true;

	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	
	C_BeingDead(PC);	// 클라에서 실행하는 것
	S_BeingDead(PC, this);	// 서버에서 실행하는 것
}

void AWCharacterBase::S_BeingDead_Implementation(AGamePlayerController* PC, APawn* Player)
{
	bIsDead = true;

	// 골드
	APlayGameMode* GameMode = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnObjectKilled(this, LastHitBy);
	}
	
	//캐릭터 리스폰
	APlayGameState* GameState = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (PC && GameState && GameMode && HasAuthority())
	{
		PC->S_SetCurrentRespawnTime();

		PC->S_CountRespawnTime();
		
		GameState->GameManagedActors.Remove(Player);
		
		FTimerHandle RespawnTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle,
			[Player, PC, GameMode]()
			{
				GameMode->RespawnPlayer(Player, PC);
			}, GameState->RespawnTime, false);

		FTimerHandle SpectatorCamera;
		GetWorld()->GetTimerManager().SetTimer(SpectatorCamera,
			[this, PC]()
			{
				PC->PossessToSpectatorCamera(FollowCamera->GetComponentLocation(), FollowCamera->GetComponentRotation());
			}, 1.3f, false);
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

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, [this]()
	{
		Destroy();
	}, 1.3f, false);
}

void AWCharacterBase::C_BeingDead_Implementation(AGamePlayerController* PC)
{
	// CheckDistance 셋타이머 끄기
	if (CheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
	}
	
	// 클라에서 리스폰 위젯 출력
	if (PC != nullptr)
	{
		//리스폰 실행
		PC->ShowRespawnWidget();
	}

	//죽으면 카메라 움직임에 메쉬 따라 움직이지 않게 하기
	this->bUseControllerRotationYaw = false;

	// 죽으면 카메라 회전 못하게 하기
	PC->SetIgnoreLookInput(true);
}

//포인트 데미지 주는 함수
void AWCharacterBase::HandleApplyPointDamage(FHitResult LastHit)
{
	if (HasAuthority())
	{
		NM_SpawnHitEffect(LastHit.Location);
		
		AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
		if (PC)
		{
			AGamePlayerState* PState = PC->GetPlayerState<AGamePlayerState>();
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
		EnterCombat();
		
		LastHitBy = EventInstigator;
		
		AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
		if (PS)
		{
			PS->Server_ApplyDamage(DamageAmount, EventInstigator);
		}
	}

	AWCharacterBase* AttackChar = Cast<AWCharacterBase>(DamageCauser);
	if (TowerWithCharacterInside && AttackChar && TowerWithCharacterInside->OverlappingActors.Contains(AttackChar))
	{
		TowerWithCharacterInside->OverlappingActors.Swap(0, TowerWithCharacterInside->OverlappingActors.Find(AttackChar));
	}
	
	return DamageAmount;
}

void AWCharacterBase::HandleGameEnd()
{
	GetCharacterMovement()->DisableMovement();

	if (IsLocallyControlled())
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC) PC->DisableInput(PC);

		PrimaryActorTick.bCanEverTick = false;
	}
}

void AWCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterDamage);
	DOREPLIFETIME(ThisClass, CharacterTeam);
	DOREPLIFETIME(ThisClass, ControllerRotation);
	DOREPLIFETIME(ThisClass, Yaw);
	DOREPLIFETIME(ThisClass, IsCombat);
}