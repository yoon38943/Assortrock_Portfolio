#include "Game/SelectCharacterPlayerController.h"

#include "SelectMapGameState.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/SelectMapUserWidget.h"

void ASelectCharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority()) return;
	
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ASelectCharacterPlayerController::ToInGameLoading_Implementation()
{
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, ToInGameLoadingWidgetClass);
	if (LoadingWidget)
	{
		LoadingWidget->AddToViewport(0);
	}
}

void ASelectCharacterPlayerController::Server_ControllerIsReady_Implementation()
{
	ASelectMapGameState* GS = Cast<ASelectMapGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->CheckPlayerIsReady(this);
	}
}

void ASelectCharacterPlayerController::BackToLobby_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("클라이언트 게임 로비로 복귀 중..."));
	ClientTravel("/Game/Portfolio/Menu/L_MainLobby", TRAVEL_Absolute);
}

void ASelectCharacterPlayerController::PlayerStateInfoReady_Implementation()
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

void ASelectCharacterPlayerController::UpdatePlayerWidget_Implementation()
{
	USelectMapUserWidget* Widget = Cast<USelectMapUserWidget>(SelectCharacterWidget);
	if (Widget)
	{
		Widget->UpdateWidget();
	}
}