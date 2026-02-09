#pragma once
#include "E_LocomotionDirection.generated.h"

UENUM(BlueprintType)
enum class E_LocomotionDirection :uint8
{
	Forward,
	Left,
	Right,
	Backward
};