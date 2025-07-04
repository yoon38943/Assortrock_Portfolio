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
#include "Components/ProgressBar.h"
#include "Components/SceneComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Gimmick/Tower.h"
#include "Minions/WMinionsCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "PersistentGame/GamePlayerController.h"
#include "PersistentGame/GamePlayerState.h"
#include "PersistentGame/PlayGameMode.h"
#include "PersistentGame/PlayGameState.h"
#include "UI/PlayerHPInfoBar.h"


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
		for (AActor* Actor : GS->ManagedActors)
		{
			if (!IsValid(Actor) || Actor == this) continue;

			float DistSqr = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
			bool bShouldShow = DistSqr <= VisibleDistanceSqr;

			SetWidgetVisible(Actor, bShouldShow);
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

	HPInfoBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPInfoBar"));
	HPInfoBarComponent->SetupAttachment(GetRootComponent());
	HPInfoBarComponent->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	HPInfoBarComponent->SetVisibility(false);

	bAlwaysRelevant = true;
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

	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->ManagedActors.Add(this);
	}
	
	if (!HasAuthority())
	{
		if (IsLocallyControlled())
			HPInfoBarComponent->SetVisibility(true);
		SetHPInfoBarColor();
		SetHPPercentage();
		ShowNickName();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			CheckTimerHandle,
			this,
			&ThisClass::SetVisibleWidgetDistance,
			0.2f,
			true
		);
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

void AWCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterDamage);
	DOREPLIFETIME(ThisClass, CharacterTeam);
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
}

void AWCharacterBase::NM_StopPlayMontage_Implementation()
{
	StopAnimMontage();
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
	
	//죽음 메세지 출력
	auto Message = FString::Printf(TEXT("Dead"));
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, Message);

	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	
	C_BeingDead(PC);	// 클라에서 실행하는 것
	S_BeingDead(PC, this);	// 서버에서 실행하는 것
}

void AWCharacterBase::S_BeingDead_Implementation(AGamePlayerController* PC, APawn* Player)
{
	bIsDead = true;
	
	//캐릭터 리스폰
	APlayGameState* GameState = Cast<APlayGameState>(GetWorld()->GetGameState());
	APlayGameMode* GameMode = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
	if (PC && GameState && GameMode && HasAuthority())
	{
		PC->S_SetCurrentRespawnTime();

		PC->S_CountRespawnTime();
		
		GameState->ManagedActors.Remove(Player);
		
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

	//ServerPlayMontage(HitAnimMontage);
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
