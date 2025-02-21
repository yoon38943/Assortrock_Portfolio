#include "SelectGameState.h"
#include "SelectGameMode.h"
#include "SelectPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"	


void ASelectGameState::BeginPlay()
{
	Super::BeginPlay();

	if (ASelectPlayerController* SelectPlayerController = Cast<ASelectPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		SelectPC = SelectPlayerController;
	}
	else UE_LOG(LogTemp, Error, TEXT("ASelectGameStateSelectPC 생성 실패!"));
	
}


void ASelectGameState::M_UpdateTeamSlot_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("ASelectGameStateM_UpdateTeamSlot"));
	SelectPC->UpdateTeamSlot(WidgetID,UserNickName);
}

void ASelectGameState::M_UnCheckTeamSlot_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("ASelectGameStateM_UnCheckTeamSlot"));

	SelectPC->UnCheckTeamSlot(UncheckWidgetID);
}

void ASelectGameState::M_UpdateCharName_Implementation(int32 MWidgetID, const FText& MCharName)
{
	UpdateChar(MWidgetID,MCharName);
}

void ASelectGameState::UpdateChar(int32 MWidgetID, const FText& MCharName)
{
	if (SelectPC!=nullptr)
	{
		SelectPC->UpdateChar(MWidgetID, MCharName);
	}
}

void ASelectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASelectGameState,WidgetID);
	DOREPLIFETIME(ASelectGameState,UncheckWidgetID);
	DOREPLIFETIME(ASelectGameState,SelectTime);
	DOREPLIFETIME(ASelectGameState,UserNickName);
}

void ASelectGameState::OnRep_WidgetID()
{
	UE_LOG(LogTemp, Error, TEXT("ASelectGameStateOnRep_WidgetID()"));

	M_UpdateTeamSlot();
}

void ASelectGameState::OnRep_UncheckWidgetID()
{
	UE_LOG(LogTemp, Error, TEXT("ASelectGameStateOnRep_UncheckWidgetID()"));

	if (UncheckWidgetID != 0)
	{
		M_UnCheckTeamSlot();
	}
	else return;
}

void ASelectGameState::OnRep_SelectTime()
{
	UE_LOG(LogTemp, Error, TEXT("ASelectGameStateOnRep_SelectTime()"));

	if (SelectTime == 0)
	{
		SelectPC->S_MapTravel();
		SelectPC->WorldTime = SelectTime;
	}
	else
	{
		SelectPC->WorldTime = SelectTime;
	}
}
