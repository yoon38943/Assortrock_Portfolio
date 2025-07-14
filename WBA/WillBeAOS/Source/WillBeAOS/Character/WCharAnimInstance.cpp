#include "WCharAnimInstance.h"
#include "WCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


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
		Pitch = Character->Pitch;
		Yaw = Character->Yaw;

		TurningInPlace = Character->TurningInPlace;
	}
	
	if (WCharMovementComponent)
	{
		WCharVelocity = WCharMovementComponent->Velocity;
		WCharSpeed = UKismetMathLibrary::VSizeXY(WCharVelocity);
		
		if (UKismetMathLibrary::VSizeXY(WCharMovementComponent->GetCurrentAcceleration()) > 0)
			WIsAccelerating = true;
		else
			WIsAccelerating = false;

		float CurveValue = UAnimInstance::GetCurveValue(TEXT("FullBody"));
		if (CurveValue > 0.f)
		{
			FullBody = true;
		}
		else
			FullBody = false;

		WCharInAir = WCharMovementComponent->IsFalling();
	}
}

void UWCharAnimInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, IsInCombat);
}