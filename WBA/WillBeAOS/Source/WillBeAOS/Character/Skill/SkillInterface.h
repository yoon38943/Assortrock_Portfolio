#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkillType.h"
#include "SkillInterface.generated.h"

UINTERFACE()
class USkillInterface : public UInterface { GENERATED_BODY() };

class ISkillInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
	void ActivateSkill(ESkillSlot SkillSlot);
};
