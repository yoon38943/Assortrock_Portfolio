#include "WMinionsAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/CharacterMovementComponent.h"

void AWMinionsAIController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (MinionBT != nullptr)
		{
			FTimerHandle DelayTimer;
			GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &AWMinionsAIController::StartBTWithDelay, 0.3f, false);
		}
	}
}

void AWMinionsAIController::MulticastCorrectPosition_Implementation(FVector CorrectLocation)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		ControlledPawn->SetActorLocation(CorrectLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	// **Velocity 강제 초기화 (클라에서도 실행됨)**
	if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(ControlledPawn->GetMovementComponent()))
	{
		MoveComp->StopMovementImmediately();
	}
}

void AWMinionsAIController::StartBTWithDelay()
{
	// 위치 보정 후 BT 실행
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		MulticastCorrectPosition(ControlledPawn->GetActorLocation());
	}

	if (MinionBT)
	{
		RunBehaviorTree(MinionBT);
	}
}
