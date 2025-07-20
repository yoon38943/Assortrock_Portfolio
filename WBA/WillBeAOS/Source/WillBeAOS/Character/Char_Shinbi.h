#pragma once

#include "CoreMinimal.h"
#include "WCharacterBase.h"
#include "Char_Shinbi.generated.h"

UCLASS()
class WILLBEAOS_API AChar_Shinbi : public AWCharacterBase
{
	GENERATED_BODY()

public:
	AChar_Shinbi();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	
	TArray<AActor*> GetTartgetInCenter();

	virtual void SkillQ(const FInputActionValue& Value) override;
	virtual void Server_SkillQ() override;
	virtual void NM_SkillPlayMontage(UAnimMontage* SkillMontage) override;
	void SpawnWolfSkill();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWolf> WolfClass;
};
