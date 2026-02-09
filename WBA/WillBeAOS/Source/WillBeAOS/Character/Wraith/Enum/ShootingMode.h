#pragma once
#include "ShootingMode.generated.h"

UENUM(BlueprintType)
enum class ShootingMode : uint8
{
	NonCombat,
	Rifle,
	Sniping,
	Bomb
};