#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecallWidget.generated.h"

UCLASS()
class WILLBEAOS_API URecallWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	float RecallTime;

	UPROPERTY(BlueprintReadWrite)
	float RemainTime;

	void StartRecalling();
};
