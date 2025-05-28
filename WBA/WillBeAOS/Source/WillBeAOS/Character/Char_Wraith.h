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

protected:
	UPROPERTY(BlueprintReadWrite)
	FVector EnemyLocation;

	UFUNCTION(NetMulticast, reliable)
	void NM_HitParticle(FVector HitLocation);

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void WraithAttack(FVector EnemyLocationParam);
};
