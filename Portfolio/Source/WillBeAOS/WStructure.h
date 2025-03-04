#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"  // UObject 관련 기능 사용 가능
#include "WStructure.generated.h"


USTRUCT(BlueprintType)
struct FPlayerValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_TeamID TeamValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsReady;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class APawn> WPawnClass;

	
	// 기본 생성자
	explicit FPlayerValue(): TeamValue(E_TeamID::Neutral), IsReady(false), WPawnClass(nullptr) {}

	// 커스텀 생성자
	FPlayerValue(E_TeamID IntValue, bool BoolValue, TSubclassOf<class APawn> PawnClass): TeamValue(IntValue), IsReady(BoolValue) ,WPawnClass(PawnClass){}
};