#include "Character/Char_Shinbi.h"

#include "AOSActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PersistentGame/PlayGameState.h"


AChar_Shinbi::AChar_Shinbi()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AChar_Shinbi::BeginPlay()
{
	Super::BeginPlay();
	
}

void AChar_Shinbi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() && IsLocallyControlled())
	{
		TArray<AActor*> TargetEnemy = GetTartgetInCenter();

		if (TargetEnemy.Num() > 0)
		{
			bIsEnemyLockOn = true;
			AttackTarget = TargetEnemy;
		}
		else
		{
			bIsEnemyLockOn = false;
			TArray<AActor*> Empty;
			AttackTarget = Empty;
		}
	}

}

TArray<AActor*> AChar_Shinbi::GetTartgetInCenter()
{
	FVector ActorLocation = GetRootComponent()->GetComponentLocation();
	FRotator ActorRotation = GetRootComponent()->GetComponentRotation();
	FVector ForwardVector = ActorRotation.Vector();

	FVector TraceStart = ForwardVector * 100 + ActorLocation;
	FVector TraceEnd = TraceStart;

	TArray<FHitResult> Hits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	if (APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState()))
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
						ActorsToIgnore.Add(Ally);
				}

				// AAOSActor 중 아군 채널 제외
				AAOSActor* InGameActor = Cast<AAOSActor>(Ally);
				if (InGameActor)
				{
					if (IsValid(InGameActor) && InGameActor->TeamID == TeamID)
						ActorsToIgnore.Add(Ally);
				}
			}
		}
	}
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	bool AttackSuccess = UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWorld(),
		TraceStart,
		TraceEnd,
		FVector(50.f, 50.f, 90.f),
		FRotator(0.f),
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.f
	);

	TArray<AActor*> AllTarget;

	if (AttackSuccess)
	{
		for (int32 i = 0; i < Hits.Num(); ++i)
		{
			const FHitResult& Elem = Hits[i];
			bool bIsLast = (i == Hits.Num() - 1);
			
			if (!Cast<AAOSCharacter>(Elem.GetActor()) && !Cast<AAOSActor>(Elem.GetActor())) continue;

			AllTarget.AddUnique(Elem.GetActor());

			if (bIsLast)
			{
				return AllTarget;
			}
		}
	}
	
	return AllTarget;
}

void AChar_Shinbi::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

