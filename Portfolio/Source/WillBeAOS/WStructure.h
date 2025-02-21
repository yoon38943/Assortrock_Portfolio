#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"  // UObject 관련 기능 사용 가능
#include "WStructure.generated.h"


USTRUCT(BlueprintType)
struct FPlayerValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TeamValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsReady;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APawn> WPawnClass;

	
	// 기본 생성자
	explicit FPlayerValue(): TeamValue(0), IsReady(false), WPawnClass(nullptr) {}

	// 커스텀 생성자
	FPlayerValue(int32 IntValue, bool BoolValue, TSubclassOf<class APawn> PawnClass): TeamValue(IntValue), IsReady(BoolValue) ,WPawnClass(PawnClass){}
};