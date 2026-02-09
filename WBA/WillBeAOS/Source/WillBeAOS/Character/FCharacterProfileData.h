#pragma once
#include "Engine/DataTable.h"
#include "FCharacterProfileData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterProfileData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FCharacterProfileData() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ProfileImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter> SelectCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter> InGameCharacter;
};