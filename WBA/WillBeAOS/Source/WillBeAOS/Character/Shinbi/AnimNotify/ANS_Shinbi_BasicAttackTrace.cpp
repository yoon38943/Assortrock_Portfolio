#include "Character/Shinbi/AnimNotify/ANS_Shinbi_BasicAttackTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"


void UANS_Shinbi_BasicAttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	HitActors.Empty();
	PrevStartLocation = MeshComp->GetSocketLocation("Sword_Root");
}

void UANS_Shinbi_BasicAttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	FVector CurrentStart = MeshComp->GetSocketLocation("Sword_Root");
	FVector CurrentEnd = MeshComp->GetSocketLocation("Sword_Tip");

	FVector BladeDirection = (CurrentStart - CurrentEnd).GetSafeNormal();

	FQuat BladeRotation = FRotationMatrix::MakeFromZ(BladeDirection).ToQuat();

	float BladeLength = FVector::Distance(CurrentStart, CurrentEnd);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(MeshComp->GetOwner());

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	MeshComp->GetWorld()->SweepMultiByObjectType(
		HitResults,
		PrevStartLocation,
		CurrentStart,
		BladeRotation,
		ObjectParams,
		FCollisionShape::MakeCapsule(15.f, BladeLength * 0.5f),
		Params);

	for (FHitResult Result : HitResults)
	{
		AActor* HitActor = Result.GetActor();
		if (HitActors.Contains(HitActor))
		{
			continue;
		}

		HitActors.Add(HitActor);

		FGameplayEventData EventData;
		EventData.Target = HitActor;
		EventData.Instigator = MeshComp->GetOwner();

		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Result);
		EventData.TargetData.Add(TargetData);

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, EventData);
	}

	PrevStartLocation = CurrentStart;
}

void UANS_Shinbi_BasicAttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation);

	HitActors.Empty();
}
