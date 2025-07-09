#include "Char_Wraith.h"

#include "AOSActor.h"
#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PersistentGame/PlayGameState.h"

void AChar_Wraith::WraithAttack_Implementation(const FVector& Start, const FVector& End)
{
	/*FVector WorldLocation, WorldDirection;
	FVector2D ScreenCenter;
	int32 ViewportX, ViewportY;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	PC->GetViewportSize(ViewportX, ViewportY);
	ScreenCenter = FVector2D(ViewportX, ViewportY) * 0.5f;

	PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);

	FVector Start = WorldLocation;
	FVector End = Start + WorldDirection * 1500.0f;*/

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GS)
	{
		for (AActor* Ally : GS->ManagedActors)
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

	TArray<FHitResult> Hits;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GS)
	{
		for (AActor* Ally : GS->ManagedActors)
		{
			if (IsValid(Ally))
			{
				// AAOSCharacter 중 아군 채널 제외
				if (Ally->IsA<AAOSCharacter>())
				{
					AAOSCharacter* InGameChar = Cast<AAOSCharacter>(Ally);
					if (IsValid(InGameChar) && InGameChar->TeamID == TeamID)
						QueryParams.AddIgnoredActor(Ally);
				}

				// AAOSActor 중 아군 채널 제외
				if (Ally->IsA<AAOSActor>())
				{
					AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
					if (IsValid(InGameActor) && InGameActor->TeamID == TeamID)
						QueryParams.AddIgnoredActor(Ally);
				}
			}
		}
	}

	FCollisionObjectQueryParams ObjectQuery;
	ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

	GetWorld()->SweepMultiByObjectType(
		Hits,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ObjectQuery,
		FCollisionShape::MakeSphere(50.f),
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
		
		if (!IsValid(IsAOSChar) && !IsValid(IsAOSActor))
		{
			continue;
		}

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
