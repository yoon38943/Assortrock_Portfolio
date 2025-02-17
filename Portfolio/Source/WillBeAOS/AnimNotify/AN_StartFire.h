#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_StartFire.generated.h"

DECLARE_DELEGATE(FOnStartFire);

UCLASS()
class WILLBEAOS_API UAN_StartFire : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	FOnStartFire OnStartFire;
};
