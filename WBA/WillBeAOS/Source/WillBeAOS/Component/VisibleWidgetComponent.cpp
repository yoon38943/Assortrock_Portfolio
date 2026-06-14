#include "Component/VisibleWidgetComponent.h"
#include "Components/WidgetComponent.h"


UVisibleWidgetComponent::UVisibleWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVisibleWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AWCharacterBase>(GetOwner());

	if (GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		HpInterface = Cast<IVisibleSightInterface>(GetOwner());
		if (HpInterface)
		{
			HealthBarWidget = HpInterface->GetHPWidgetComponent();
			if (HealthBarWidget)
			{
				CachedHealthBar = Cast<UPlayerHPInfoBar>(HealthBarWidget->GetWidget());
			}
		}
		
		APawn* pawn = Cast<APawn>(GetOwner());
		if (pawn)
		{
			if (!HealthBarWidget) return;
			
			HealthBarWidget->SetVisibility(false);
		}
		
		GetWorld()->GetTimerManager().SetTimer(
			visibleTimerHandle,
			this,
			&UVisibleWidgetComponent::CheckVisibility,
			0.2f,
			true);
	}
}

void UVisibleWidgetComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HealthBarWidget && !GetOwner()->HasAuthority() &&
		OwnerCharacter && !OwnerCharacter->IsLocallyControlled())
	{
		SetWidgetScaleByDistance();
	}
}

void UVisibleWidgetComponent::CheckVisibility()
{
	if (!HealthBarWidget) return;
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		float distSquared = FVector::DistSquared(GetOwner()->GetActorLocation(), PC->GetPawn()->GetActorLocation());

		bool bIsVisible = (distSquared <= FMath::Square(SightRadius));
		if (HealthBarWidget->IsVisible() != bIsVisible)
		{
			HealthBarWidget->SetVisibility(bIsVisible);
		}
	}
}

void UVisibleWidgetComponent::SetWidgetScaleByDistance()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	float Distance = FVector::Distance(GetOwner()->GetActorLocation(), PlayerPawn->GetActorLocation());

	float MinDistance = 200.f;
	float MaxDistance = 2500.f;
	float MinScale = 0.6f;
	float MaxScale = 1.f;

	float Scale = FMath::GetMappedRangeValueClamped(
		FVector2D(MinDistance, MaxDistance),
		FVector2D(MaxScale, MinScale),
		Distance
	);
	
	if (CachedHealthBar)
	{
		CachedHealthBar->SetBarScale(Scale);
	}

	float BaseHeight = OwnerCharacter->BaseWidgetHeight;
	float RelativeHeight = BaseHeight + (1.f - Scale) * 120.f;
	
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, RelativeHeight));
}
