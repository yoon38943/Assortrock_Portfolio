#pragma once
#include "WalkSpeedStruct.generated.h"

USTRUCT(BlueprintType)
struct FMovementSpeedStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float MaxWalkSpeed = 500;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float MaxAcceleration = 600;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float BrakingDeceleration = 1200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float BrakingFrictionFactor = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float BrakingFriction = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	bool UseSeperateBrakingFriction = true;
};
