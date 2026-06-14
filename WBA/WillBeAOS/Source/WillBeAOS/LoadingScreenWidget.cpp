#include "LoadingScreenWidget.h"

#include "Components/Button.h"
#include "Game/WGameInstance.h"


void ULoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->bShowMouseCursor = true;        // 마우스 커서 표시
		PC->bEnableClickEvents = true;      // 마우스 클릭 이벤트 활성화
		PC->bEnableMouseOverEvents = true;  // 마우스 오버 이벤트 활성화
	}

	BackButton->OnClicked.AddDynamic(this, &ThisClass::BackButtonClicked);
}

void ULoadingScreenWidget::BackButtonClicked()
{
	UWGameInstance* GI = Cast<UWGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		GI->LeaveSession();
	}
}

void ULoadingScreenWidget::UpdateSessionMaxPlayers(int32 MaxPlayersNum)
{
	SessionMaxPlayers = MaxPlayersNum;
}

void ULoadingScreenWidget::UpdateSessionPlayersNum(int32 CurrentPlayersNum)
{
	CurrentSessionPlayers = CurrentPlayersNum;
}