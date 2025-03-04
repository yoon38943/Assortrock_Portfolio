#pragma once

#include "CoreMinimal.h"
#include "WCharacterHUD.h"
#include "WEnumFile.h"
#include "Game/WGameState.h"
#include "GameFramework/PlayerController.h"
#include "WPlayerController.generated.h"

class AWCharacterBase;
class UWCharacterHUD;
class UTowerNexusHPWidget;
class UUserWidget;

UCLASS()
class WILLBEAOS_API AWPlayerController : public APlayerController
{
	GENERATED_BODY()

	//이김 위젯
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf< UUserWidget> WinScreenClass;
	//패배 위젯
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf< UUserWidget> LoseScreenClass;
	//죽음 위젯
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf< UUserWidget> DeathScreenClass;
	// 리스폰 위젯
	UPROPERTY(EditAnywhere, Category = Widget)
	TSubclassOf<UUserWidget> RespawnScreenClass;

	// UserWidget 클래스의 타입을 저장하는 변수
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> UserWidgetClass;

	//WorldGameState(Tower, Nexus, GameTime)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameStateClass;

public:
	UWCharacterHUD* PlayerHUD;
	UTowerNexusHPWidget* GamePlayHUD;
	UUserWidget* RespawnScreen;
	AWCharacterBase* AWC;
	
public:	//상점 관련
	UPROPERTY(BlueprintReadWrite, Category = "Store")
	bool IsOpenedStore;
public:
	
	// ---- 귀환 관련 함수 ----
	bool IsRecalling = false;
	FTimerHandle RecallTimerHandle;
	UPROPERTY(BlueprintReadWrite)
	float RecallTime = 8.f;

	void StartRecall();
	void CancelRecall();
	void CompleteRecall();
	UFUNCTION(Server, Reliable)
	void RecallToBase();

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> RecallWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UUserWidget* RecallWidget;
	

public://리스폰
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentRespawnTime;
	FTimerHandle RestartTimer;
	FTimerHandle RespawnTimerHandle;
public://리스폰 함수(PlayerController->GameHasEnded())
	UFUNCTION(NetMulticast, Reliable)
	void GameEnded(bool bIsWinner);
	void ShowRespawnWidget();
	void UpdateRespawnWidget();
	void HideRespawnWidget();
	
	void OnGameStateChanged(E_GamePlay CurrentGameState);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	virtual void OnPossess(APawn* InPawn) override;
};