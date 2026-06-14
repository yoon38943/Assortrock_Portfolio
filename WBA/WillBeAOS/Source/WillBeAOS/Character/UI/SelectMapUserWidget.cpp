#include "Character/UI/SelectMapUserWidget.h"

#include "Game/WGameInstance.h"


void USelectMapUserWidget::UpdateWidget_Implementation()
{
}

void USelectMapUserWidget::LogTeam(E_TeamID Team)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), Team == E_TeamID::Blue ? TEXT("블루팀") : TEXT("레드팀"));
}
