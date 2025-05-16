#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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


protected:

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector WCharVelocity;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float WCharSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool WIsAccelerating;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool WCharInAir;
	UPROPERTY(BlueprintReadOnly, Category = "FullBody")
	bool FullBody;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
