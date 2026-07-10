#pragma once

#include "CoreMinimal.h"
#include "WCharacterBase.h"
#include "Shinbi/Skill/SkillDataTable.h"
#include "Char_Shinbi.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQSkillUsed, FString, CharacterName, float, SkillCollTime);

UCLASS()
class WILLBEAOS_API AChar_Shinbi : public AWCharacterBase
{
	GENERATED_BODY()

public:
	AChar_Shinbi();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	TArray<AActor*> GetTartgetInCenter();

	// 스킬 관련 함수
	FOnQSkillUsed OnQSkillUsed;
	
	// Q 스킬
	UAnimMontage* SkillQMontage;
	UPROPERTY(Replicated)
	bool bEnableQSkill = true;
	FSkillDataTable* QSkill;

	FTimerHandle S_SkillQTimer;
	float QSkillCooldownTime;
	
	UFUNCTION(Server, Reliable)
	virtual void Server_SkillQ();
	UFUNCTION(NetMulticast, Reliable, Category = "Combat")
	void NM_SkillPlayMontage(UAnimMontage* SkillMontage);
	void SpawnWolfSkill();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWolf> WolfClass;
};
