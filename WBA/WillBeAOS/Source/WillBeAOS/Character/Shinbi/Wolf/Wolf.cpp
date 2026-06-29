#include "Character/Shinbi/Wolf/Wolf.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Character/AOSActor.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AWolf::AWolf()
{
	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AWolf::LaunchWolf(AActor* InInstigator)
{
	WolfInstigator = InInstigator;
    
	PrevLocation = GetActorLocation();

	if (DashMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DashMontage);
		}
	}
}

void AWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDisappear) return;

	DashToForward(DeltaTime);
	
	if (HasAuthority())
	{
		CheckPathHit();
	}
}

void AWolf::DashToForward(float DeltaTime)
{
	if (TotalMoveDistance >= DashEndDistance)
	{
		Explosion(GetActorLocation());
		return;
	}
	
	TotalMoveDistance += DashSpeed * DeltaTime;
	FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * DashSpeed * DeltaTime);
	SetActorLocation(NewLocation);
}

void AWolf::Explosion(const FVector& ImpactLocation)
{
	Disappear();

	Explode_Particle_Multicast(ImpactLocation);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(WolfInstigator);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		ImpactLocation,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(ExplosionRadius),
		Params
	);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 16, Overlaps.Num() > 0 ? FColor::Green : FColor::Red, false, 3.f);

	for (const auto& Overlap : Overlaps)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Overlap.GetActor()->GetName());
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor) continue;

		IGetInfoInterface* TargetTeam = Cast<IGetInfoInterface>(HitActor);
		IGetInfoInterface* SourceTeam = Cast<IGetInfoInterface>(GetInstigator());

		if (!TargetTeam || !SourceTeam) continue;
		if (TargetTeam->GetTeamID() == SourceTeam->GetTeamID()) continue;

		FGameplayEventData EventData;
		EventData.Target = HitActor;
		EventData.Instigator = WolfInstigator;

		FGameplayAbilityTargetData_ActorArray* TargetData = new FGameplayAbilityTargetData_ActorArray;
		TargetData->TargetActorArray.Add(HitActor);
		EventData.TargetData.Add(TargetData);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), EventDamageTag, EventData);
	}
}

void AWolf::Explode_Particle_Multicast_Implementation(const FVector& ImpactLocation)
{
	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ExplodeParticle,
		ImpactLocation,
		GetActorRotation()
	);
}

void AWolf::Disappear_Implementation()
{
	bIsDisappear = true;
	
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	GetMesh()->SetVisibility(false);
	
	if (TrailEffect)
		TrailEffect->DeactivateSystem();

	SetLifeSpan(3.f);
}

void AWolf::CheckPathHit()
{	
	FVector CurrentLocation = GetActorLocation();

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(WolfInstigator);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	GetWorld()->SweepMultiByObjectType(
		HitResults,
		PrevLocation,
		CurrentLocation,
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeBox(FVector(70, 20, 40)),
		Params
	);

	/*DrawDebugBox(
		GetWorld(),
		CurrentLocation,
		FVector(70, 20, 40),
		GetActorForwardVector().ToOrientationQuat(),
		HitResults.Num() > 0 ? FColor::Green : FColor::Red,
		false,
		5.0f
	);*/

	for (FHitResult HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor || HitActors.Contains(HitActor)) continue;

		IGetInfoInterface* TargetTeam = Cast<IGetInfoInterface>(HitActor);
		IGetInfoInterface* SourceTeam = Cast<IGetInfoInterface>(GetInstigator());

		if (!TargetTeam || !SourceTeam) continue;
		if (TargetTeam->GetTeamID() == SourceTeam->GetTeamID()) continue;

		HitActors.Add(HitActor);

		if (Cast<AWCharacterBase>(HitActor) || Cast<AAOSActor>(HitActor))
		{
			Explosion(HitResult.ImpactPoint);
			return;
		}

		FGameplayEventData EventData;
		EventData.Target = HitActor;
		EventData.Instigator = WolfInstigator;

		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		EventData.TargetData.Add(TargetData);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), EventDamageTag, EventData);
	}

	PrevLocation = CurrentLocation;
}
