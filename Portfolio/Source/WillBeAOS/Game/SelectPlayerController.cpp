#include "SelectPlayerController.h"
#include "SelectGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


ASelectPlayerController::ASelectPlayerController()
{
	SetShowMouseCursor(true);
}

void ASelectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && SelectWidgetClass != nullptr)
	{
		SelectWidget = CreateWidget<UUserWidget>(this, SelectWidgetClass);
		if (SelectWidget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerControllerSelectWidget 생성 성공!"));
			
			SelectWidget->AddToViewport(0);
			// 플레이어 컨트롤러의 입력모드를 UI 전용으로 설정. ( 마우스 커서가 뷰포트에 Lock 걸리지 않게 )
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this, SelectWidget, EMouseLockMode::DoNotLock);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ASelectPlayerControllerSelectWidget 생성 실패!"));
		}
	}
}

void ASelectPlayerController::S_ServerUpdateChecked_Implementation(int32 MWidgetID, int32 MUnCheckWidgetID, const FText& MNickName)
{
		if (ASelectGameMode* SelectGM = Cast<ASelectGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerControllerServerUpdateChecked!"));
			SelectGM->SetTeamSlotIsChecked(MWidgetID,MUnCheckWidgetID,MNickName);
		}
}

bool ASelectPlayerController::S_ServerUpdateChecked_Validate(int32 MWidgetID,int32 MUnCheckWidgetID, const FText& MCharName)
{
	ASelectGameMode* SelectGM = Cast<ASelectGameMode>(GetWorld()->GetAuthGameMode());
	if (!SelectGM)
	{
		return false;
	}
	return true;
}

void ASelectPlayerController::SetCharName(int32 MWidgetID, const FText& MCharName)
{
	S_SetCharName(MWidgetID, MCharName);
}

void ASelectPlayerController::HandleWidgetUpdate(int32 MWidgetID, const FText& MUserNickName)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerControllerHandleWidgetUpdate!"));
	if (OldWidgetID == 0)
	{
		S_ServerUpdateChecked(MWidgetID,0,MUserNickName);
		OldWidgetID = MWidgetID;
		OwnWidgetID = MWidgetID;
	}
	else
	{
		S_ServerUpdateChecked(MWidgetID,OldWidgetID,MUserNickName);
		OldWidgetID = MWidgetID;
		OwnWidgetID = MWidgetID;
	}
}

void ASelectPlayerController::S_SetCharName_Implementation(int32 MWidgetID, const FText& MCharName)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerControllerSetCharName!"));
		if (ASelectGameMode* SelectGM = Cast<ASelectGameMode>(GetWorld()->GetAuthGameMode()))
		{
			SelectGM->UpdateCharName(MWidgetID,MCharName);
		}
}

void ASelectPlayerController::CancelWidget(int32 MCancelWidget)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerControllerCancelWidget"));
	S_ServerUpdateChecked(0,MCancelWidget,FText::GetEmpty());
}

void ASelectPlayerController::UnCheckTeamSlot_Implementation(int32 OldWidget)
{
	UE_LOG(LogTemp, Error, TEXT("ASelectPlayerControllerUnCheckTeamSlot_Implementation"));
}

void ASelectPlayerController::UpdateTeamSlot_Implementation(int32 MWidgetID, const FText& MUserNickName)
{
	UE_LOG(LogTemp, Error, TEXT("ASelectPlayerControllerUpdateTeamSlot_Implementation"));
}

void ASelectPlayerController::UpdateChar_Implementation(int32 MWidgetID, const FText& MCharName)
{
	UE_LOG(LogTemp, Error, TEXT("ASelectPlayerControllerUpdateChar_Implementation!"));
}

void ASelectPlayerController::S_MapTravel_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("ASelectPlayerController S_MapTravel_Implementation()"));
	if (ASelectGameMode* SelectGM = Cast<ASelectGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerControllerS_MapTravel"));
		SelectGM->CheckMatch();
	}
}

void ASelectPlayerController::S_IsReady_Implementation(E_TeamID TeamID, bool Ready, TSubclassOf<APawn> PawnClass)
{
	if (ASelectGameMode* SelectGM = Cast<ASelectGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ASelectPlayerController S_IsReady"));
		APlayerController* PC = Cast<APlayerController>(this);
		if (PC)
		{
			SelectGM->SetIsReady(PC, TeamID ,Ready, PawnClass);
		}
	}
}
