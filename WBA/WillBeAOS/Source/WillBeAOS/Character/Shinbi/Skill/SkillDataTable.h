#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillDataTable.generated.h"

USTRUCT(BlueprintType)
struct FSkillDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkillCooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* SkillMontage;
};
