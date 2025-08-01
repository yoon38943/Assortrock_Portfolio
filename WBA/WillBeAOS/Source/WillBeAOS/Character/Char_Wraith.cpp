#include "Char_Wraith.h"

#include "AOSActor.h"
#include "CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gimmick/Projectile.h"
#include "Gimmick/Tower.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PersistentGame/GamePlayerController.h"
#include "PersistentGame/GamePlayerState.h"
#include "PersistentGame/PlayGameState.h"
#include "Wraith/Wraith_Projectile_Enhanced.h"
#include "Wraith/Wraith_Projectile_Normal.h"



AChar_Wraith::AChar_Wraith()
{
	ScopeAttackCameraPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScopeAttackCameraPoint"));
	ScopeAttackCameraPoint->SetupAttachment(RootComponent);
}

void AChar_Wraith::WraithAttack_Implementation(const FVector& Start, const FVector& Direction, const FVector& SocketLocation)
{
	if (HasAuthority())
	{
		if (Projectile_Normal)
		{
			FVector TraceStart = Start;
			FVector TraceEnd = TraceStart + Direction * TargettingTraceLength;
			
			FHitResult HitActor;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);

			if (GS)
			{
				for (auto WeakActor : GS->GameManagedActors)
				{
					if (WeakActor)
					{
						AActor* Ally = WeakActor;
				
						if (!Ally) continue;
			
						// AAOSCharacter 중 아군 채널 제외
						AAOSCharacter* InGameChar = Cast<AAOSCharacter>(Ally);
						if (InGameChar)
						{
							if (InGameChar->TeamID == TeamID)
							{
								QueryParams.AddIgnoredActor(Ally);
							}
						}

						// AAOSActor 중 아군 채널 제외
						AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
						if (InGameActor)
						{
							if (IsValid(InGameActor) && InGameActor->TeamID == TeamID)
							{
								QueryParams.AddIgnoredActor(Ally);
							}
						}
					}
				}
			}
			
			FCollisionObjectQueryParams ObjectQuery;
			ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
			ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

			bool AttackSuccess = GetWorld()->LineTraceSingleByObjectType(
				HitActor,
				TraceStart,
				TraceEnd,
				ObjectQuery,
				QueryParams
			);

			FVector HitLocation;
			if (AttackSuccess)
			{
				float Damage = 0;
				AController* Ctrl = Cast<AController>(GetController());
				if (Ctrl)
				{
					AGamePlayerState* PS = Ctrl->GetPlayerState<AGamePlayerState>();
					if (PS)
					{
						Damage = PS->CPower;
					}
				}

				if (Damage > 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), Damage);
					UGameplayStatics::ApplyPointDamage(
					HitActor.GetActor(),
					Damage,
					GetActorForwardVector(),
					HitActor,
					GetInstigatorController(),
					this,
					UDamageType::StaticClass()
					);
				}
				
				HitLocation = HitActor.ImpactPoint;

				NM_HitEffect(HitLocation);
			}
			else
			{
				HitLocation = TraceEnd;
			}
			
			FVector ProjectileWay = HitLocation - SocketLocation;
			FRotator ProjectileWayRot = ProjectileWay.Rotation();

			NM_SpawnProjectile(SocketLocation, HitLocation, ProjectileWayRot, false);
		}
	}
}

void AChar_Wraith::BeginPlay()
{
	Super::BeginPlay();
	
	GS = Cast<APlayGameState>(GetWorld()->GetGameState());

	if (SkillDataTable)
	{
		QSkill = SkillDataTable->FindRow<FSkillDataTable>(FName("QSkill"), TEXT(""));
	}
	
	if (QSkill)
	{
		SkillQMontage = QSkill->SkillMontage;
	}
}

void AChar_Wraith::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (!HasAuthority())
	UE_LOG(LogTemp, Warning, TEXT("%f"), TargettingTraceLength);*/
	if (bIsDead) return;

	if (!HasAuthority() && IsLocallyControlled())
	{
		AActor* TargetEnemy = CheckTargettingInCenter();

		if (LastTarget != TargetEnemy)
		{
			if (IsValid(TargetEnemy))
			{
				bIsEnemyLockOn = true;
				if (AttackTarget.Num() > 0)
				{
					TArray<AActor*> DeleteActors;
					for (auto& Actor : AttackTarget)
					{
						DeleteActors.Add(Actor);
					}

					for (auto& Actor : DeleteActors)
					{
						AttackTarget.Remove(Actor);
					}
				}
				LastTarget = TargetEnemy;
				AttackTarget.AddUnique(LastTarget);
			}
			else
			{
				bIsEnemyLockOn = false;
				if (AttackTarget.Num() > 0)
				{
					TArray<AActor*> DeleteActors;
					for (auto& Actor : AttackTarget)
					{
						DeleteActors.Add(Actor);
					}

					for (auto& Actor : DeleteActors)
					{
						AttackTarget.Remove(Actor);
					}
				}
				LastTarget = nullptr;
			}
		}
	}
}

AActor* AChar_Wraith::CheckTargettingInCenter()
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
	FVector TraceEnd = TraceStart + WorldDirection * TargettingTraceLength;

	FHitResult HitActor;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GS)
	{
		for (TWeakObjectPtr<AActor> WeakActor : GS->CachedActors)
		{
			if (WeakActor.IsValid())
			{
				AActor* Ally = WeakActor.Get();
				
				if (!IsValid(Ally)) continue;
			
				// AAOSCharacter 중 아군 채널 제외
				AAOSCharacter* InGameChar = Cast<AAOSCharacter>(Ally);
				if (InGameChar)
				{
					if (InGameChar->TeamID == TeamID)
						QueryParams.AddIgnoredActor(Ally);
				}

				// AAOSActor 중 아군 채널 제외
				AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
				if (InGameActor)
				{
					if (IsValid(InGameActor) && InGameActor->TeamID == TeamID)
						QueryParams.AddIgnoredActor(Ally);
				}
			}
		}
	}

	FCollisionObjectQueryParams ObjectQuery;
	ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

	bool AttackSuccess = GetWorld()->LineTraceSingleByObjectType(
		HitActor,
		TraceStart,
		TraceEnd,
		ObjectQuery,
		QueryParams
	);

	if (AttackSuccess)
	{
		if (!Cast<AAOSCharacter>(HitActor.GetActor()) && !Cast<AAOSActor>(HitActor.GetActor()))
		{
			return nullptr;
		}
		
		return HitActor.GetActor();
	}
	else
	{
		return nullptr;
	}
}

void AChar_Wraith::Attack()
{
	if (bIsZoomIn)
	{
		SkillQAttack();
	}
	else
	{
		Super::Attack();
	}
	
}

void AChar_Wraith::Behavior()
{
	EnterCombat();
	
	CombatComp->SetCollisionMesh(GetMesh());
	if (CombatComp != nullptr)
	{
		//공격중이 아닐시
		if (CombatComp->IsCombatEnable() == false && CanAttack == true)
		{
			//공격중 활성화
			CombatComp->SetCombatEnable(true);
			//콤보 로직
			if ((CombatComp->GetAttackCount()) < AttackMontages.Num())
			{
				NM_Behavior(CombatComp->GetAttackCount());
				AttackFire();
				CanAttack = false;
				FTimerHandle AttackTimer;
				GetWorld()->GetTimerManager().SetTimer(AttackTimer,
					[this]()
					{
						CanAttack = true;
					},
					0.77f,
					false);
				CombatComp->AddAttackCount(1);
				if (CombatComp->GetAttackCount() >= AttackMontages.Num())
				{
					CombatComp->ResetCombo();
				}
			}
		}
	}
}

void AChar_Wraith::NM_HitEffect_Implementation(const FVector& HitLocation)
{
	if (HitParticle && !HasAuthority())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(0.7f),
			true);
	}
}

void AChar_Wraith::AttackFire_Implementation()
{
	// 블루프린트 정의
}

void AChar_Wraith::SkillQ()
{
	Super::SkillQ();

	AGamePlayerController* PC = Cast<AGamePlayerController>(GetController());
	if (PC && PC->IsRecalling)
	{
		PC->Server_CancelRecall();
	}

	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		if (!bIsZoomIn && PS->SkillQEnable == true)
		{
			ZoomInScope();
		}
		else if (bIsZoomIn && PS->SkillQEnable == true)
		{
			ZoomOutScope();
		}
	}
}

void AChar_Wraith::NM_SpawnProjectile_Implementation(const FVector& SocketLocation, const FVector& HitLocation, const FRotator& ProjectileRot, bool SkillBullet)
{
	if (!HasAuthority())
	{
		if (SkillBullet == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), FVector::Dist(SocketLocation, HitLocation));
			AWraith_Projectile_Enhanced* Proj = GetWorld()->SpawnActor<AWraith_Projectile_Enhanced>(Projectile_Enhanced, SocketLocation, ProjectileRot);
			if (Proj)
			{
				Proj->SetOwner(this);
				Proj->SetInstigator(this);
				Proj->DistanceVector = FVector::Dist(SocketLocation, HitLocation);
			}
		}
		else
		{
			AWraith_Projectile_Normal* Proj = GetWorld()->SpawnActor<AWraith_Projectile_Normal>(Projectile_Normal, SocketLocation, ProjectileRot);
			if (Proj)
			{
				Proj->SetOwner(this);
				Proj->SetInstigator(this);
				Proj->DistanceVector = FVector::Dist(SocketLocation, HitLocation);
			}
		}
	}
}

void AChar_Wraith::ZoomInScope()
{
	bIsZoomIn = true;
	SetZoomInBool(bIsZoomIn);
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		ChangeCharacterSpeed(PS->CSpeed / 3);
	}
	TargettingTraceLength = 4000.f;
	GetWorld()->GetTimerManager().SetTimer(ZoomTimer, this, &ThisClass::UpdateZoom, 0.01f, true);
}

void AChar_Wraith::ZoomOutScope()
{
	bIsZoomIn = false;
	SetZoomInBool(bIsZoomIn);
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		ChangeCharacterSpeed(PS->CSpeed);
	}
	TargettingTraceLength = 1500.f;
	GetWorld()->GetTimerManager().SetTimer(ZoomTimer, this, &ThisClass::UpdateZoom, 0.01f, true);
}

void AChar_Wraith::SetZoomInBool_Implementation(bool bZoomIn)
{
	bIsZoomIn = bZoomIn;
}

void AChar_Wraith::UpdateZoom()
{
	float TargetFOV = bIsZoomIn ? 45.f : 90.f;
	float CurrentFOV = FollowCamera->FieldOfView;
	float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, GetWorld()->DeltaTimeSeconds, 10.f);

	FollowCamera->SetFieldOfView(NewFOV);

	if (FMath::Abs(NewFOV - TargetFOV) <= 0.5f)
	{
		FollowCamera->SetFieldOfView(TargetFOV);
		GetWorld()->GetTimerManager().ClearTimer(ZoomTimer);
	}
}

void AChar_Wraith::ChangeCharacterSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	C_ChangeCharacterSpeed(Speed);
}

void AChar_Wraith::C_ChangeCharacterSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AChar_Wraith::SkillQAttack()
{
	if (bIsDead) return;
	
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		if (PS->SkillQEnable == true)
		{
			PS->SkillQCoolTime = QSkill->SkillCooldownTime;
			PS->UsedQSkill();

			S_SkillQAttack();
			ZoomOutScope();
		}
	}
}

void AChar_Wraith::CallRecall()
{
	if (bIsZoomIn)
	{
		ZoomOutScope();
	}
	
	Super::CallRecall();
}

void AChar_Wraith::S_SkillQAttack_Implementation()
{
	AGamePlayerState* PS = Cast<AGamePlayerState>(GetPlayerState());
	if (PS)
	{
		if (PS->ServerSkillQEnable == true)
		{
			PS->SkillQCoolTime = QSkill->SkillCooldownTime;
			PS->Server_UsedQSkill();

			TargettingTraceLength = 4000.f;
			BP_EnhancedAttack();
			NM_PlayMontage(SkillQMontage);
		}
	}
}

void AChar_Wraith::BP_EnhancedAttack_Implementation()
{
	// 블루 프린트 정의
}

void AChar_Wraith::EnhancedAttack_Implementation(const FVector& Start, const FVector& Direction, const FVector& SocketLocation)
{
	if (HasAuthority())
	{
		if (Projectile_Enhanced)
		{
			FVector TraceStart = Start;
			FVector TraceEnd = TraceStart + Direction * TargettingTraceLength;
			
			FHitResult HitActor;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);

			if (GS)
			{
				for (auto WeakActor : GS->GameManagedActors)
				{
					if (WeakActor)
					{
						AActor* Ally = WeakActor;
				
						if (!Ally) continue;
			
						// AAOSCharacter 중 아군 채널 제외
						AAOSCharacter* InGameChar = Cast<AAOSCharacter>(Ally);
						if (InGameChar)
						{
							if (InGameChar->TeamID == TeamID)
							{
								QueryParams.AddIgnoredActor(Ally);
							}
						}

						// AAOSActor 중 아군 채널 제외
						AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
						if (InGameActor)
						{
							if (IsValid(InGameActor) && InGameActor->TeamID == TeamID)
							{
								QueryParams.AddIgnoredActor(Ally);
							}

							ATower* Tower = Cast<ATower>(InGameActor);
							if (Tower)
							{
								QueryParams.AddIgnoredActor(Ally);
							}
						}
					}
				}
			}
			
			FCollisionObjectQueryParams ObjectQuery;
			ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
			ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

			bool AttackSuccess = GetWorld()->LineTraceSingleByObjectType(
				HitActor,
				TraceStart,
				TraceEnd,
				ObjectQuery,
				QueryParams
			);

			FVector HitLocation;
			if (AttackSuccess)
			{
				float Damage = 0;
				AController* Ctrl = Cast<AController>(GetController());
				if (Ctrl)
				{
					AGamePlayerState* PS = Ctrl->GetPlayerState<AGamePlayerState>();
					if (PS)
					{
						Damage = PS->CPower * 2.f;
					}
				}

				if (Damage > 0)
				{
					UGameplayStatics::ApplyPointDamage(
					HitActor.GetActor(),
					Damage,
					GetActorForwardVector(),
					HitActor,
					GetInstigatorController(),
					this,
					UDamageType::StaticClass()
					);
				}
				
				HitLocation = HitActor.ImpactPoint;

				NM_HitEffect(HitLocation);
			}
			else
			{
				HitLocation = TraceEnd;
			}
			
			FVector ProjectileWay = HitLocation - SocketLocation;
			FRotator ProjectileWayRot = ProjectileWay.Rotation();

			UE_LOG(LogTemp, Warning, TEXT("%s"), *HitLocation.ToString())

			NM_SpawnProjectile(SocketLocation, HitLocation, ProjectileWayRot, true);

			TargettingTraceLength = 1500;
		}
	}
}

void AChar_Wraith::NM_PlayMontage_Implementation(UAnimMontage* SkillMontage)
{
	if (!HasAuthority())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(SkillMontage);
	}
}

void AChar_Wraith::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TargettingTraceLength);
}
