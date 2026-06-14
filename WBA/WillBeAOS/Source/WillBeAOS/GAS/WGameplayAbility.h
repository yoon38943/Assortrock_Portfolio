#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WGameplayAbility.generated.h"

UCLASS()
class WILLBEAOS_API UWGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
};
