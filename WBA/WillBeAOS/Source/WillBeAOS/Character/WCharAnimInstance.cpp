#include "WCharAnimInstance.h"
#include "WCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UWCharAnimInstance::NativeInitializeAnimation()
{
	// 캐릭터 레퍼런스 저장
	WCharBase = Cast<AWCharacterBase>(TryGetPawnOwner());
	if (WCharBase != nullptr)
	{
		WCharMovementComponent = WCharBase->GetCharacterMovement();
	}
}

// 애니메이션 업데이트 루프
void UWCharAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{

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
