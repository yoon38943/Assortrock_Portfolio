#pragma once

#include "CoreMinimal.h"
#include "Character/WCharacterBase.h"
#include "Char_Wraith.generated.h"

UCLASS()
class WILLBEAOS_API AChar_Wraith : public AWCharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "HitParticle")
	UParticleSystem* HitParticle;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void WraithAttack(FVector EnemyLocation);
	
};
