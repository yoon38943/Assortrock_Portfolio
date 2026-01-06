#include "WCharAnimInstance.h"
#include "WCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PersistentGame/GamePlayerController.h"


void UWCharAnimInstance::NativeInitializeAnimation()
{
	WCharBase = Cast<AWCharacterBase>(TryGetPawnOwner());
	if (WCharBase != nullptr)
	{
		WCharMovementComponent = WCharBase->GetCharacterMovement();
	}
}

void UWCharAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	AWCharacterBase* Character = Cast<AWCharacterBase>(TryGetPawnOwner());
	if (Character)
	{
		IsInCombat = Character->IsCombat;
		
		TurningInPlace = Character->TurningInPlace;
		
		if (TurningInPlace == E_TurningInPlace::E_NotTurning)
		{
			Pitch = Character->Pitch;
			RawYawDelta = Character->Yaw;
			Yaw = FMath::FInterpTo(Yaw, RawYawDelta, DeltaSeconds, 10.0f);
		}
		else if (TurningInPlace != E_TurningInPlace::E_NotTurning)
		{
			Yaw = FMath::FInterpTo(Yaw, 0.f, DeltaSeconds, 5.0f);
		}

		AGamePlayerController* PC = Cast<AGamePlayerController>(Character->GetController());
		if (PC)
		{
			bIsRecalling = PC->IsRecalling;
		}
	}
	
	if (WCharMovementComponent)
	{
		// Velocity (속도)
		Velocity = WCharMovementComponent->Velocity;
		Velocity2D = Velocity * FVector(1.f, 1.f, 0.f);
		WCharSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		
		// Acceleration (가속도)
		Acceleration = WCharMovementComponent->GetCurrentAcceleration();
		Acceleration2D = Acceleration * FVector(1.f, 1.f, 0.f);
		if (UKismetMathLibrary::VSizeXY(WCharMovementComponent->GetCurrentAcceleration()) > 0)
			WIsAccelerating = true;
		else
			WIsAccelerating = false;

		// FullBody (풀바디 커브)
		float CurveValue = GetCurveValue(TEXT("FullBody"));
		if (CurveValue > 0.f)
		{
			FullBody = true;
		}
		else
			FullBody = false;
	}
}