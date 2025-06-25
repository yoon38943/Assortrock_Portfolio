#include "PersistentGame/GamePlayerController.h"

#include "PlayGameState.h"
#include "Character/UI/SelectMapUserWidget.h"


void AGamePlayerController::StartCharacterSelectPhase()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AGamePlayerController::Server_ControllerIsReady_Implementation()
{
	APlayGameState* GS = Cast<APlayGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->CheckPlayerIsReady(this);
	}
}

void AGamePlayerController::PlayerStateInfoReady_Implementation()
{
	SelectCharacterWidget = CreateWidget<UUserWidget>(this, SelectCharacterWidgetClass);
	if (SelectCharacterWidget)
	{
		SelectCharacterWidget->AddToViewport(0);
	}

	// 위젯까지 띄운 후에 준비 클라 준비 완료됐다고 보고하기
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 준비 완료"));
	Server_ControllerIsReady();
}

void AGamePlayerController::UpdatePlayerWidget_Implementation()
{
	USelectMapUserWidget* Widget = Cast<USelectMapUserWidget>(SelectCharacterWidget);
	if (Widget)
	{
		Widget->UpdateWidget();
	}
}

void AGamePlayerController::BackToLobby_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 게임 로비로 복귀 중..."));
	ClientTravel("/Game/Portfolio/Menu/L_MainLobby", TRAVEL_Absolute);
}

void AGamePlayerController::ToInGameLoading_Implementation()
{
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, ToInGameLoadingWidgetClass);
	if (LoadingWidget)
	{
		LoadingWidget->AddToViewport(0);
	}
}

