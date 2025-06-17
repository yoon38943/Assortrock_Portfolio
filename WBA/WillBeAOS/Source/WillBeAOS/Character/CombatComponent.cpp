#include "CombatComponent.h"
#include "WCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Minions/WMinionsCharacterBase.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicatedByDefault(true);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Health);
}

int32 UCombatComponent::GetAttackCount()
{
	return AttackCount;
}

void UCombatComponent::AddAttackCount(int32 Val)
{
	AttackCount += Val;
}

void UCombatComponent::ResetCombo()
{
	AttackCount = 0;
}

bool UCombatComponent::IsCombatEnable()
{
	if (CombatEnable)
	{
		return true;
	}
	else { return false; }
}

void UCombatComponent::SetCombatEnable(bool Val)
{
	CombatEnable = Val;
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = Max_Health;
}

void UCombatComponent::HandleTakeDamage(float WDamage)
{
		if (Health > 0)
		{
			if (WDamage > Health)
				Health = 0;
			Health -= WDamage;
		}
	
		if (Health <= 0)
		{
			SetIsDead(true);
			DelegateDead.ExecuteIfBound();
		}
}

void UCombatComponent::SetIsDead(bool Val)
{
	IsDead = Val;
}

bool UCombatComponent::GetIsDead()
{
	return IsDead;
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsCollisionEnabled && GetOwner()->HasAuthority())
	{
		CollisionTrace();
	}
}

void UCombatComponent::SetCollisionMesh(UPrimitiveComponent* PrimComp)
{
	CollisionMeshComponent = PrimComp;
}

void UCombatComponent::CollisionTrace()
{
	//HitResults 배열 선언
	TArray<FHitResult> OutHits = {};

	AWCharacterBase* Char = Cast<AWCharacterBase>(GetOwner());
	if (Char)
	{
		HalfSize = FVector(50, 50, Char->GetMesh()->Bounds.BoxExtent.Z);
	}
	AWMinionsCharacterBase* MinionChar = Cast<AWMinionsCharacterBase>(GetOwner());
	if (MinionChar)
	{
		HalfSize = FVector(30, 30, MinionChar->GetMesh()->Bounds.BoxExtent.Z);
	}
	
	//SphereTraceMultForObjects함수로 트레이스
	bool Hit = UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWorld(),
		GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100,
		GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100,
		HalfSize,
		Orientation,
		ObjectTypes,
		false,
		AlreadyHitActors,
		EDrawDebugTrace::ForOneFrame,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.f
		);

	if (Hit)
	{
		for (const FHitResult& LastHit : OutHits)
		{
			AActor* HitActor = LastHit.GetActor();
			if (HitActor && !AlreadyHitActors.Contains(HitActor))
			{
				// 새로운 히트 오브젝트인 경우
				UE_LOG(LogTemp, Log, TEXT("Hit: %s"), *LastHit.GetActor()->GetName());
				// AlreadyHitActors에 추가
				AlreadyHitActors.Add(HitActor);
				//델리게이트 함수 호출
				DelegatePointDamage.Broadcast(LastHit);
			}
		}
	}
}

void UCombatComponent::EnableCollision()
{
	if (!GetOwner()->HasAuthority()) return;
	
	ClearHitActor();
	IsCollisionEnabled = true;
}

void UCombatComponent::DisableCollision()
{
	IsCollisionEnabled = false;
}

void UCombatComponent::ClearHitActor()
{
	AlreadyHitActors.Empty();
	AlreadyHitActors.Add(GetOwner());
}