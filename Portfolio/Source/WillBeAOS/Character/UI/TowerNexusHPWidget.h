#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerNexusHPWidget.generated.h"

class UTextBlock;

UCLASS()
class WILLBEAOS_API UTowerNexusHPWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	class AWGameState* AWGS;//게임스테이트

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Tower")
	TObjectPtr<class UProgressBar>FriendTowerProgress;//아군 타워 진행상황
	UFUNCTION(BlueprintPure, Category = "Tower")
	float SetTowerProgress();//타워 진행상황 받아오는 함수
	//추후 피아 식별 후 수정
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Nexus")
	TObjectPtr<class UProgressBar>FriendNexusHealth;//아군 타워 진행상황
	UFUNCTION(BlueprintPure, Category = "Nexus")
	float SetNexusHealth();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Tower")
	TObjectPtr<class UProgressBar>RedTowerProgress;//아군 타워 진행상황
	UFUNCTION(BlueprintPure, Category = "Tower")
	float SetRedTowerProgress();//타워 진행상황 받아오는 함수
	//추후 피아 식별 후 수정
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Nexus")
	TObjectPtr<class UProgressBar>RedNexusHealth;//아군 타워 진행상황
	UFUNCTION(BlueprintPure, Category = "Nexus")
	float SetRedNexusHealth();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "GameTimer")
	UTextBlock* GameTimer;
	UFUNCTION(Blueprintpure, meta = (BindWidget), Category = "GameTimer")
	FText UpdateGameTimer();
};
