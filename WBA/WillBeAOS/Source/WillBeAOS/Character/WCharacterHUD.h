#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PersistentGame/PlayGameState.h"
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
	APlayGameState* AWGS;//게임스테이트
	UPROPERTY(BlueprintReadOnly, Category = "PlayerState")
	AGamePlayerState* AWPS;

	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void UpdateCharacter();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Health")
	TObjectPtr<class UProgressBar>HealthBar;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthBarPercentage();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "GamePoint")
	UTextBlock* KillPoint;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "GamePoint")
	UTextBlock* DeathPoint;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Power;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* AdditionalHealth;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Defence;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Speed;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Stat")
	UTextBlock* Gold;
	UFUNCTION(Blueprintpure, meta = (BindWidget), Category = "Stat")
	FText SetGold();
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