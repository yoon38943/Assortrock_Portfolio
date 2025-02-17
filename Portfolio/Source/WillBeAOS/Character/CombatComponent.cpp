#include "CombatComponent.h"
#include "Kismet/KismetSystemLibrary.h"
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

	if (IsCollisionEnabled)
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

	//SphereTraceMultForObjects함수로 트레이스
	bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		CollisionMeshComponent->GetSocketLocation(StartSocket),
		CollisionMeshComponent->GetSocketLocation(EndSocket),
		Radius,
		ObjectTypes,
		false,
		AlreadyHitActors,
		EDrawDebugTrace::ForOneFrame,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
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