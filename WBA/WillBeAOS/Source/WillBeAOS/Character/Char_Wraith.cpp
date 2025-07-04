#include "Char_Wraith.h"

#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void AChar_Wraith::WraithAttack_Implementation(FVector EnemyLocationParam)
{
	if(EnemyLocationParam.IsZero()) return;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add((UEngineTypes::ConvertToObjectType(ECC_Pawn)));
	ObjectTypes.Add((UEngineTypes::ConvertToObjectType(ECC_WorldStatic)));

	TArray<AActor*> IgnoreActors;
	FHitResult HitResult;

	FVector Start = GetMesh()->GetSocketLocation(FName(TEXT("Muzzle_01")));
	FVector End = ((EnemyLocation - Start) * 0.001f) + EnemyLocation ;

	bool TraceSuccess = UKismetSystemLibrary::LineTraceSingleForObjects(
	this,
	Start,
	End,
	ObjectTypes,
	false,
	IgnoreActors,
	EDrawDebugTrace::None,
	HitResult,
	true
	);

	if (TraceSuccess)
	{
		NM_HitParticle(HitResult.Location);
		HandleApplyPointDamage(HitResult);
	}
}

void AChar_Wraith::NM_HitParticle_Implementation(FVector HitLocation)
{
	UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(0.7f),
			true
			);
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
