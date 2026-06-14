#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class WILLBEAOS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	/******************************************/
	/*                  Main                  */
	/******************************************/
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY()
	class UWGameInstance* GameInstance;

	void BackToLoginMenu();
	void SwitchToMainWidget();
	
public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* MatchingStartButton;

	void ShowMainSwitcher();

	UFUNCTION()
	void MatchingStartButtonClicked();

	UFUNCTION()
	void MatchmakingCompleted(bool bWasSuccessful);

	/******************************************/
	/*                Login                   */
	/******************************************/
	
	UPROPERTY(meta=(BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta=(BindWidget))
	UButton* ExitGameButton;
	
	UFUNCTION()
	void LoginBtnClicked();

	UFUNCTION()
	void ExitGameBtnClicked();

	UFUNCTION(BlueprintNativeEvent)
	void RemoveLoadingScreen();
	
	void LoginCompleted(bool bWasSuccessful, const FString& UserId, const FString& Error);

	UFUNCTION(BlueprintNativeEvent)
	void CompleteMatchingInBP(bool bWasSuccessful);
	UFUNCTION(BlueprintNativeEvent)
	void MatchingButtonClickedInBP();

	/******************************************/
	/*                Waiting                 */
	/******************************************/
private:
	UPROPERTY(meta=(BindWidget))
	class ULoginWaitingWidget* WaitingWidget;

	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowcancel = false);
};
