#include "WMinionsAnimInstance.h"
#include "WMinionsCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UWMinionsAnimInstance::NativeInitializeAnimation()
{
	// 캐릭터 레퍼런스 저장
	MinionsCharBase = Cast<AWMinionsCharacterBase>(TryGetPawnOwner());
	if (MinionsCharBase)
	{
		MinionMoveCmp = MinionsCharBase->GetCharacterMovement();
	}
}

// 애니메이션 업데이트
void UWMinionsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (MinionMoveCmp)
	{
		WMinionVelocity = MinionMoveCmp->Velocity;
		WMinionSpeed = UKismetMathLibrary::VSizeXY(WMinionVelocity);

		WMinionShouldMove = false;
		bool Acceleration = !MinionMoveCmp->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0);
		if (WMinionSpeed >= 3.f && Acceleration)
		{
			WMinionShouldMove = true;
		}

		float CurveValue = UAnimInstance::GetCurveValue(TEXT("FullBody"));
		if (CurveValue > 0.f)
		{
			FullBody = true;
		}
	}
}