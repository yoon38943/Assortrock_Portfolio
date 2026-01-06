#pragma once

#include "CoreMinimal.h"
#include "SkillType.generated.h"

UENUM(BlueprintType)
enum class ESkillSlot : uint8
{
	None = 0,
	Q,
	E,
	R
};

USTRUCT(BlueprintType)
struct FSkillUsedInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ESkillSlot SkillSlot = ESkillSlot::None;

	UPROPERTY(BlueprintReadOnly)
	float CooldownTime = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float CooldownTimeEnd = 0.f;

	// TArray에서 요소를 비교할 때 사용
	bool operator==(const ESkillSlot& InSkillID) const
	{
		return SkillSlot == InSkillID;
	}
};