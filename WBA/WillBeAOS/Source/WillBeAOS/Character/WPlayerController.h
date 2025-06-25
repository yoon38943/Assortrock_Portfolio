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

	E_TeamID PlayerTeamID;

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

public:
	UPROPERTY(ReplicatedUsing = OnRep_Countdown, BlueprintReadOnly, Category = "UI")
	int32 CountdownTime;
	UFUNCTION(Category = "UI")
	void OnRep_Countdown();
public:	//상점 관련
	UPROPERTY(BlueprintReadWrite, Category = "Store")
	bool IsOpenedStore;

	UFUNCTION(Client, Reliable)
	void SetIsOpenStore(bool CanOpen);
	
public:
	// ---- 귀환 관련 함수 ----
	UPROPERTY(Replicated)
	bool IsRecalling = false;
	FTimerHandle RecallTimerHandle;
	UPROPERTY(BlueprintReadWrite)
	float RecallTime = 8.f;

	UFUNCTION(Server, Reliable)
	void StartRecall();
	UFUNCTION(Client, Reliable)
	void ShowRecallWidget();
	UFUNCTION(Client, Reliable)
	void HiddenRecallWidget(bool IsRecallCancel);
	UFUNCTION(Server, Reliable)
	void Server_CancelRecall();
	void CancelRecall();
	void CompleteRecall();
	void RecallToBase();

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> RecallWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UUserWidget* RecallWidget;
	

public://리스폰
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentRespawnTime;
	UFUNCTION(Server, Reliable)
	void S_SetCurrentRespawnTime();
	FTimerHandle RestartTimer;
	FTimerHandle RespawnTimerHandle;

	UPROPERTY(EditAnywhere, Category = "DeadCamera")
	TSubclassOf<APawn> SpectorCamera;
	
public://리스폰 함수(PlayerController->GameHasEnded())
	UFUNCTION(NetMulticast, Reliable)
	void GameEnded(E_TeamID LoseTeam);
	void ShowRespawnWidget();
	UFUNCTION(Server, Reliable)
	void S_CountRespawnTime();
	void UpdateRespawnWidget();
	UFUNCTION(Client, Reliable)
	void C_ReplicateCurrentRespawnTime(int32 RespawnTime);
	UFUNCTION(Client, Reliable)
	void HideRespawnWidget();

	// Spectator Camera 전환
	void PossessToSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);
	
	void OnGameStateChanged(E_GamePlay CurrentGameState);
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerReady(); // 서버에 준비 완료 신호 전송
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	virtual void OnPossess(APawn* InPawn) override;
};