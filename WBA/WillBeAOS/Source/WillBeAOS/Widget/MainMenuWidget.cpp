#include "Widget/MainMenuWidget.h"

#include "LoginWaitingWidget.h"
#include "Components/Button.h"
#include "Game/WGameInstance.h"
#include "Components/WidgetSwitcher.h"


void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstance = GetGameInstance<UWGameInstance>();
	if (GameInstance)
	{
		GameInstance->OnLoginCompleted.AddUObject(this, &ThisClass::LoginCompleted);
		if (GameInstance->IsLoggedIn())
		{
			MainSwitcher->SetActiveWidgetIndex(1);
		}

		GameInstance->OnMatchmakingCompleted.AddUObject(this, &ThisClass::MatchmakingCompleted);
	}
	
	LoginButton->OnClicked.AddDynamic(this, &ThisClass::LoginBtnClicked);
	//MatchingStartButton->OnClicked.AddDynamic(this, &ThisClass::LoginBtnClicked);
	MatchingStartButton->OnClicked.AddDynamic(this, &ThisClass::MatchingStartButtonClicked);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidgetIndex(1);
	}
}

void UMainMenuWidget::MatchingStartButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Matching start"));
	if (GameInstance)
	{
		GameInstance->StartMatchmaking();
	}
	MatchingButtonClickedInBP();
}

void UMainMenuWidget::MatchmakingCompleted(bool bWasSuccessful)
{
	CompleteMatchingInBP(bWasSuccessful);
}

void UMainMenuWidget::LoginBtnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("로그인 진행..."));
	if (GameInstance && !GameInstance->IsLoggedIn() && !GameInstance->IsLoggingIn())
	{
		GameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString("Logging In..."));
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& UserId, const FString& Error)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("로그인 성공! UserId: %s"), *UserId);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("로그인 실패! Error: %s"), *Error);
	}

	SwitchToMainWidget();
}

void UMainMenuWidget::CompleteMatchingInBP_Implementation(bool bWasSuccessful)
{
}

void UMainMenuWidget::MatchingButtonClickedInBP_Implementation()
{
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowcancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitInfo(WaitInfo, bAllowcancel);
	return WaitingWidget->ClearAndGetButtonClickedEvent();
}
