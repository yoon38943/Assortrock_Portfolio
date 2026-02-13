#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "LoginWaitingWidget.generated.h"

UCLASS()
class WILLBEAOS_API ULoginWaitingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent();
	void SetWaitInfo(const FText& WaitInfo, bool bAllowCancel = false);
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WaitInfoText;
	
	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;
};
