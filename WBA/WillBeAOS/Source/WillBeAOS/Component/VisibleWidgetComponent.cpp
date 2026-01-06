#include "Component/VisibleWidgetComponent.h"
#include "Components/WidgetComponent.h"
#include "Interface/VisibleSightInterface.h"


UVisibleWidgetComponent::UVisibleWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UVisibleWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		IVisibleSightInterface* hpInterface = Cast<IVisibleSightInterface>(GetOwner());
		if (hpInterface)
		{
			playerWidget = hpInterface->GetHPWidgetComponent();
		}
		
		APawn* pawn = Cast<APawn>(GetOwner());
		if (pawn)
		{
			if (playerWidget && !pawn->IsLocallyControlled())
			{
				playerWidget->SetVisibility(false);
			}
			else
			{
				playerWidget->SetVisibility(true);
			}
		}
		
		GetWorld()->GetTimerManager().SetTimer(
			visibleTimerHandle,
			this,
			&UVisibleWidgetComponent::CheckVisibility,
			0.2f,
			true);
	}
}

void UVisibleWidgetComponent::CheckVisibility()
{
	if (!playerWidget) return;
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		float distSquared = FVector::DistSquared(GetOwner()->GetActorLocation(), PC->GetPawn()->GetActorLocation());

		bool bIsVisible = (distSquared <= FMath::Square(sightRadius));
		if (playerWidget->IsVisible() != bIsVisible)
		{
			playerWidget->SetVisibility(bIsVisible);
		}
	}
}
