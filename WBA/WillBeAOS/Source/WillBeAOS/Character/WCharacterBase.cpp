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
#include "WCharAnimInstance.h"
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
}


void AWCharacterBase::BeginPlay()
{
	Super::BeginPlay();
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
	
	if (!HasAuthority())
	{
		if (IsLocallyControlled())
			HPInfoBarComponent->SetVisibility(true);
		SetHPInfoBarColor();
		SetHPPercentage();
		ShowNickName();

		GetWorld()->GetTimerManager().SetTimer(
			CheckTimerHandle,
			this,
			&ThisClass::SetVisibleWidgetDistance,
			0.2f,
			true
		);
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

	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		GetCharacterMovement()->MaxWalkSpeed = PS->CSpeed;
	}
	
	UpdateAcceleration();

	AimOffset(DeltaTime);

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		FRotator AimRot = GetBaseAimRotation();
		FRotator ActorRot = GetActorRotation();

		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRot, ActorRot);
	
		Pitch = DeltaRot.Pitch;
		Yaw = DeltaRot.Yaw;

		if (TurningInPlace == E_TurningInPlace::E_NotTurning)
		{
			InterpAOYaw = Yaw;
		}
		TurnInPlace(DeltaTime);	
	}

	if (!HasAuthority())
	{
		AActor* AttackTarget = GetTartgetInCenter();
	
		if (AttackTarget != CurrentTarget)
		{
			if (IsValid(CurrentTarget))
			{
				// 이전 타겟의 외곽선 끄기
				auto EnemyMesh = CurrentTarget->FindComponentByClass<UMeshComponent>();
				if (IsValid(EnemyMesh))
				{
					EnemyMesh->SetRenderCustomDepth(false);
				}
			}

			if (IsValid(AttackTarget))
			{
				// 외곽선 표시하기
				auto EnemyMesh = AttackTarget->FindComponentByClass<UMeshComponent>();
				if (IsValid(EnemyMesh))
				{
					EnemyMesh->SetRenderCustomDepth(true);
				}
			}

			CurrentTarget = AttackTarget;
		}
	}
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
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &AWCharacterBase::StopMove);
		EnhancedInputComponent->BindAction(IA_Behavior, ETriggerEvent::Started, this, &AWCharacterBase::Attack);
		EnhancedInputComponent->BindAction(IA_SkillR, ETriggerEvent::Started, this, &AWCharacterBase::SkillR);
		EnhancedInputComponent->BindAction(IA_Recall, ETriggerEvent::Started, this, &ThisClass::CallRecall);
	}
}

void AWCharacterBase::AimOffset(float DeltaTime)
{
	
}

void AWCharacterBase::TurnInPlace(float DeltaTime)
{
	if (Yaw > 90.f)
	{
		TurningInPlace = E_TurningInPlace::E_TurningRight;
	}
	else if (Yaw < -90.f)
	{
		TurningInPlace = E_TurningInPlace::E_TurningLeft;
	}
	if (TurningInPlace != E_TurningInPlace::E_NotTurning)
	{
		InterpAOYaw = FMath::FInterpTo(InterpAOYaw, 0.f, DeltaTime, 5.f);
		Yaw = InterpAOYaw;
		if (FMath::Abs(Yaw) < 15.f)
		{
			TurningInPlace = E_TurningInPlace::E_NotTurning;
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

void AWCharacterBase::SetVisibleWidgetDistance()
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
		if (Character->HPInfoBarComponent && Character->HPInfoBarComponent->IsVisible() != bIsVisible)
		{
			Character->HPInfoBarComponent->SetVisibility(bIsVisible);
		}
	}
	else if (AWMinionsCharacterBase* Minion = Cast<AWMinionsCharacterBase>(Actor))
	{
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	
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

void AWCharacterBase::NM_StopPlayMontage_Implementation()
{
	StopAnimMontage();
}

AActor* AWCharacterBase::GetTartgetInCenter()
{
	FVector WorldLocation, WorldDirection;
	FVector2D ScreenCenter;
	int32 ViewportX, ViewportY;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return nullptr;

	PC->GetViewportSize(ViewportX, ViewportY);
	ScreenCenter = FVector2D(ViewportX, ViewportY) * 0.5f;

	PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);

	FVector TraceStart = WorldLocation;
	FVector TraceEnd = TraceStart + WorldDirection * 2500.0f;

	TArray<FHitResult> Hits;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQuery;
	ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

	GetWorld()->SweepMultiByObjectType(
		Hits,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ObjectQuery,
		FCollisionShape::MakeSphere(70.f),
		QueryParams
	);

	AActor* BestTarget = nullptr;
	float BestAngle = FLT_MAX;

	for (auto& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor)) continue;

		AAOSCharacter* IsAOSChar = Cast<AAOSCharacter>(HitActor);
		if (IsAOSChar)
		{
			if (IsAOSChar->TeamID == TeamID) continue;
		}

		AAOSActor* IsAOSActor = Cast<AAOSActor>(HitActor);
		if (IsAOSActor)
		{
			if (IsAOSActor->TeamID == TeamID) continue;
		}
		
		if (!IsValid(IsAOSChar) && !IsValid(IsAOSActor)) continue;

		FVector ToTarget = (HitActor->GetActorLocation() - TraceStart).GetSafeNormal();
		float Angle = FMath::Acos(FVector::DotProduct(WorldDirection, ToTarget));

		if (Angle < BestAngle)
		{
			BestTarget = HitActor;
			BestAngle = Angle;
		}
	}
	
	return BestTarget;
}

void AWCharacterBase::Attack()
{
	if (IsDead == true) return;
	
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

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		UWCharAnimInstance* Anim = Cast<UWCharAnimInstance>(AnimInstance);
		if (Anim)
		{
			Anim->IsInCombat = true;
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(CombatTimer);
	GetWorld()->GetTimerManager().SetTimer(CombatTimer, this, &ThisClass::ExitCombat, 15.f, false);
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

void AWCharacterBase::BeingDead()
{
	IsDead = true;

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

		PC->PossessToSpectatorCamera(FollowCamera->GetComponentLocation(), FollowCamera->GetComponentRotation());
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
}