#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WCharacterHUD.generated.h"

class UTextBlock;

UCLASS()
class WILLBEAOS_API UWCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FTimerHandle TimerHandle;
	FTimerHandle ErrorTimerHandle;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class AWCharacterBase* AWC;//캐릭터 받아오는 함수
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	class AWGameState* AWGS;//게임스테이트
	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
	class AWPlayerState* AWPS;

	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void UpdateCharacter();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Health")
	TObjectPtr<class UProgressBar>HealthBar;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthBarPercentage();

	//스킬데이터 구조체
	struct FSkillCooldownData
	{
		float SkillCooldown;
		float CurrentSkillCooldown;
		UProgressBar* SkillProgress;
		UTextBlock* SkillTimer;
	};

	void SetSkillTimer(FSkillCooldownData& SkillData);
	FText ShowSkillTimer(FSkillCooldownData& SkillData);
	float ShowSkillProgress(FSkillCooldownData& SkillData);

	FSkillCooldownData SkillLData;
	FSkillCooldownData SkillRData;

	//평타(LeftClick)쿨타임
	UPROPERTY(BlueprintReadWrite, Category = "Skill_L")
	float SkillLCooldown = 0.5f;
	FTimerHandle CooldownLTimerHandle;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Skill_L")
	TObjectPtr<class UProgressBar>Skill_LProgress;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Skill_L")
	UTextBlock* Skill_LTimer;
	UFUNCTION(BlueprintCallable, Category = "Skill_L")
	void SetSkillLTimer();
	UFUNCTION(BlueprintPure, Category = "Skill_L")
	FText ShowSkillLTimer();
	UFUNCTION(BlueprintPure, Category = "Skill_L")
	float ShowSkillLProgress();
	UFUNCTION(BlueprintCallable, Category = "Skill_L")
	void UpdateSkillLTimer();

	//스킬R(RightClick)쿨타임
	UPROPERTY(BlueprintReadWrite, Category = "Skill_L")
	float SkillRCooldown = 3.0f;
	FTimerHandle CooldownRTimerHandle;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Skill_R")
	TObjectPtr<class UProgressBar>Skill_RProgress;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Skill_R")
	UTextBlock* Skill_RTimer;
	UFUNCTION(BlueprintCallable, Category = "Skill_R")
	void SetSkillRTimer();
	UFUNCTION(BlueprintPure, Category = "Skill_R")
	FText ShowSkillRTimer();
	UFUNCTION(BlueprintPure, Category = "Skill_R")
	float ShowSkillRProgress();
	UFUNCTION(BlueprintCallable, Category = "Skill_R")
	void UpdateSkillRTimer();

	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Power;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* AdditionalHealth;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Defence;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Speed;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Level;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Exp;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Max_Exp;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Gold;
	UFUNCTION(Blueprintpure, meta = (BindWidget), Category = "Stat")
	FText SetGold();
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* AbilityLevel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* CurrentHP;

public:
	void SetState();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	// 위젯 초기화 시 실행되는 함수
	virtual void NativeConstruct() override;

	void TryGetPlayerState();
};