#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Shinbi_SkillDataTable.generated.h"

USTRUCT(BlueprintType)
struct FShinbi_SkillDataTable : public FTableRowBase
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
