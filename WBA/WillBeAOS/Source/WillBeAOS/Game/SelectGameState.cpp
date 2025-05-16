#include "SelectGameState.h"
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


void ASelectGameState::M_UpdateTeamSlot_Implementation(int32 WidgetID, const FText& UserNickName)
{
	if (SelectPC)
	{
		SelectPC->UpdateTeamSlot(WidgetID,UserNickName);
	}
}

void ASelectGameState::M_UnCheckTeamSlot_Implementation(int32 UnCheckWidgetID)
{
	UE_LOG(LogTemp, Error, TEXT("ASelectGameStateM_UnCheckTeamSlot"));

	SelectPC->UnCheckTeamSlot(UnCheckWidgetID);
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
	DOREPLIFETIME(ASelectGameState,SelectTime);
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
