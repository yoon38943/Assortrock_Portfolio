#include "Widget/LoginWaitingWidget.h"

#include "Components/TextBlock.h"


void ULoginWaitingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

FOnButtonClickedEvent& ULoginWaitingWidget::ClearAndGetButtonClickedEvent()
{
	CancelButton->OnClicked.Clear();
	return CancelButton->OnClicked;
}

void ULoginWaitingWidget::SetWaitInfo(const FText& WaitInfo, bool bAllowCancel)
{
	if (CancelButton)
	{
		CancelButton->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (WaitInfoText)
	{
		WaitInfoText->SetText(WaitInfo);
	}
}
