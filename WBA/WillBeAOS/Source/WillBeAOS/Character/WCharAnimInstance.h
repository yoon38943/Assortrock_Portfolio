#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WEnumFile.h"
#include "Struct_Enum/E_LocomotionDirection.h"
#include "WCharAnimInstance.generated.h"

class AWCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class WILLBEAOS_API UWCharAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	AWCharacterBase* WCharBase;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* WCharMovementComponent;

	UPROPERTY(BlueprintReadWrite, Category = MoveMent)
	E_TurningInPlace TurningInPlace;

	UPROPERTY(BlueprintReadWrite)
	bool IsInCombat = false;


protected:

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Pitch;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Yaw;
	float RawYawDelta;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRecalling;

	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	FVector Velocity;
	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	FVector Velocity2D;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float WCharSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Acceleration)
	FVector Acceleration;
	UPROPERTY(BlueprintReadOnly, Category = Acceleration)
	FVector Acceleration2D;
	UPROPERTY(BlueprintReadOnly, Category = Acceleration)
	bool WIsAccelerating;
	UPROPERTY(BlueprintReadOnly, Category = "FullBody")
	bool FullBody;

	UPROPERTY(BlueprintReadOnly)
	FRotator WorldRotation;

	UPROPERTY(BlueprintReadOnly)
	float VelocityLocomotionAngle;
	UPROPERTY(BlueprintReadOnly)
	E_LocomotionDirection LocomotionDirection;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void CaculateLocomotionDirection();
};
