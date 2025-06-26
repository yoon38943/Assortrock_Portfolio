#include "PersistentGame/GamePlayerController.h"

#include "PlayGameMode.h"
#include "PlayGameState.h"
#include "Character/UI/SelectMapUserWidget.h"
#include "Kismet/GameplayStatics.h"


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
	LoadingWidget = CreateWidget<UUserWidget>(this, ToInGameLoadingWidgetClass);
	if (LoadingWidget)
	{
		LoadingWidget->AddToViewport(0);
	}
}

void AGamePlayerController::CheckLoadedAllStreamingLevels()
{
	FName LastLevelName;
	
	APlayGameMode* GM = Cast<APlayGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		LastLevelName = FName(GM->StreamingLevelSequence.Last().GetAssetName());
	}

	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(this, LastLevelName);
	if (StreamingLevel && StreamingLevel->IsLevelLoaded() && StreamingLevel->IsLevelVisible())
	{
		// 레벨이 로드 됐으면 클라 준비 완료
		StartInGamePhase();
	}
	else
	{
		// 레벨이 클라에서 로드 되었는지 재확인
		FTimerHandle ReCheckTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ReCheckTimerHandle, this, &ThisClass::CheckLoadedAllStreamingLevels, 0.1f, false);
	}
}

void AGamePlayerController::StartInGamePhase()
{
	// 인게임 전환 로딩창 비활성화
	if (LoadingWidget && LoadingWidget->IsInViewport())
	{
		LoadingWidget->RemoveFromParent();
	}

	// 다시 마우스 안보이도록
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;

	// 클라 준비 됐다고 신호 보내기


	// 캐릭터 소환되면 인게임 위젯 붙이기
}
