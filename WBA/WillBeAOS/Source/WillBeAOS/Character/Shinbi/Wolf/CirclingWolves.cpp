#include "Character/Shinbi/Wolf/CirclingWolves.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ACirclingWolves::ACirclingWolves()
{
	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = true;
}

void ACirclingWolves::InitWolves(AActor* InOwner, const float InStartAngle, const float InCircleRadius, const float InLifeTime)
{
	WolvesOwner = InOwner;
	CurrentAngle = InStartAngle;
	CircleRadius = InCircleRadius;

	SetLifeSpan(InLifeTime);
}

void ACirclingWolves::BeginPlay()
{
	Super::BeginPlay();
}

void ACirclingWolves::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!WolvesOwner) return;

	UpdateCirclingPosition(DeltaTime);
}

void ACirclingWolves::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ExplodeParticle,
		GetActorLocation(),
		GetActorRotation()
	);

	Super::EndPlay(EndPlayReason);
}

void ACirclingWolves::UpdateCirclingPosition(float DeltaTime)
{
	CurrentAngle -= TurnSpeed * DeltaTime;
	if (CurrentAngle < 0.f) CurrentAngle += 360.f;

	float RadAngle = FMath::DegreesToRadians(CurrentAngle);

	FVector OwnerLocation = WolvesOwner->GetActorLocation();
	FVector Offset = FVector(FMath::Cos(RadAngle) * CircleRadius, FMath::Sin(RadAngle) * CircleRadius, 0.f);

	FVector NewLocation = OwnerLocation + Offset;
	SetActorLocation(NewLocation);

	FVector Forward = FVector(FMath::Sin(RadAngle), -FMath::Cos(RadAngle), 0.f);
	SetActorRotation(Forward.Rotation());
}
