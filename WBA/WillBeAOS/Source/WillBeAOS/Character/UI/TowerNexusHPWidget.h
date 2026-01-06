#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerNexusHPWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class WILLBEAOS_API UTowerNexusHPWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	class APlayGameState* AWGS;//게임스테이트

public:	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Nexus")
	TObjectPtr<UProgressBar>BlueNexusHealth;//아군 넥서스 진행상황
	UFUNCTION(BlueprintPure, Category = "Nexus")
	float SetBlueTeamNexusHealth();
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Nexus")
	TObjectPtr<UProgressBar>RedNexusHealth;//아군 타워 진행상황
	UFUNCTION(BlueprintPure, Category = "Nexus")
	float SetRedTeamNexusHealth();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "GameTimer")
	UTextBlock* GameTimer;
	UFUNCTION(Blueprintpure, meta = (BindWidget), Category = "GameTimer")
	FText UpdateGameTimer();

	// 팀별 전투 상황
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* BlueTeamKillPoints;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* RedTeamKillPoints;

	UFUNCTION()
	void UpdateTeamKillPoints(int32 Blue, int32 Red);
};
