#pragma once

#include "CoreMinimal.h"
#include "WCharacterBase.h"
#include "Shinbi/Skill/Shinbi_SkillDataTable.h"
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

	FShinbi_SkillDataTable* QSkill;
	virtual void SkillQ() override;
	UFUNCTION(Server, Reliable)
	virtual void Server_SkillQ();
	UFUNCTION(NetMulticast, Reliable, Category = "Combat")
	void NM_SkillPlayMontage(UAnimMontage* SkillMontage);
	void SpawnWolfSkill();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWolf> WolfClass;
};
