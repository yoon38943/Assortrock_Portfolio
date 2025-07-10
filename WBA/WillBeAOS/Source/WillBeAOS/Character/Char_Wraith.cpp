#include "Char_Wraith.h"

#include "AOSActor.h"
#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PersistentGame/PlayGameState.h"

void AChar_Wraith::WraithAttack_Implementation(const FVector& Start, const FVector& End)
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GS)
	{
		for (AActor* Ally : GS->GameManagedActors)
		{
			if (IsValid(Ally))
			{
				// AAOSCharacter 중 아군 채널 제외
				AAOSCharacter* InGameChar = Cast<AAOSCharacter>(Ally);
				if (InGameChar && InGameChar->TeamID == TeamID)
					QueryParams.AddIgnoredActor(Ally);
			
				// AAOSActor 중 아군 채널 제외
				AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
				if (InGameActor && InGameActor->TeamID == TeamID)
					QueryParams.AddIgnoredActor(Ally);
			}
		}
	}

	FCollisionObjectQueryParams ObjectQuery;
	ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

	bool AttackSuccess = GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		Start,
		End,
		ObjectQuery,
		QueryParams
	);

	if (AttackSuccess)
	{
		NM_HitParticle(HitResult.Location);
		HandleApplyPointDamage(HitResult);
	}
}

void AChar_Wraith::NM_HitParticle_Implementation(FVector HitLocation)
{
	if (!HitParticle) return;
	
	UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(0.7f),
			true
			);
}

void AChar_Wraith::BeginPlay()
{
	Super::BeginPlay();
	
	GS = Cast<APlayGameState>(GetWorld()->GetGameState());
}

void AChar_Wraith::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		AActor* TargetEnemy = CheckTargettingInCenter();

		if (IsValid(TargetEnemy))
		{
			bIsEnemyLockOn = true;
		}
		else
		{
			bIsEnemyLockOn = false;
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
	FVector TraceEnd = TraceStart + WorldDirection * 1600.0f;

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

void AChar_Wraith::Behavior()
{
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

void AChar_Wraith::AttackFire_Implementation()
{
	// 블루프린트 내 구현
}
