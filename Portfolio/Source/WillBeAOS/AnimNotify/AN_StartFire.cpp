#include "AnimNotify/AN_StartFire.h"


void UAN_StartFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	OnStartFire.Execute();
}
