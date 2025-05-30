// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UEndGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReturnToMainLobbyButton)
	{
		ReturnToMainLobbyButton->OnClicked.AddDynamic(this, &UEndGameWidget::ReturnToMainLobby);
	}
}

void UEndGameWidget::ReturnToMainLobby()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->ClientTravel("/Game/Portfolio/Menu/L_MainLobby", ETravelType::TRAVEL_Absolute);
	}
}
