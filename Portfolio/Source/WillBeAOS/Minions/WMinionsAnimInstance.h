#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WMinionsAnimInstance.generated.h"

class AWMinionsCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class WILLBEAOS_API UWMinionsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	AWMinionsCharacterBase* MinionsCharBase;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* MinionMoveCmp;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector WMinionVelocity;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float WMinionSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool WMinionShouldMove;
	UPROPERTY(BlueprintReadOnly, Category = FullBody)
	bool FullBody;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
