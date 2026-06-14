#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWAbilityInputID : uint8
{
	None					UMETA(DisplayName = "None"),
	BasicAttack				UMETA(DisplayName = "BasicAttack"),
	AbilityOne				UMETA(DisplayName = "AbilityOne"),
	AbilityTwo				UMETA(DisplayName = "AbilityTwo"),
	AbilityThree			UMETA(DisplayName = "AbilityThree"),
	AbilityFour				UMETA(DisplayName = "AbilityFour"),
	Confirm					UMETA(DisplayName = "Confirm"),
	Cancel					UMETA(DisplayName = "Cancel")
};