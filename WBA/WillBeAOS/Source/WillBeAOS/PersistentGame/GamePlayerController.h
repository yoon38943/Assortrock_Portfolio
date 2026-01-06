#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.generated.h"

enum class E_GamePlay : uint8;
class UTowerNexusHPWidget;
class UWCharacterHUD;
enum class E_TeamID : uint8;

UCLASS()
class WILLBEAOS_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()


	
	// ---------------------------------------------
	// 플레이 캐릭터 선택 페이즈
	// ---------------------------------------------
public:
	void StartCharacterSelectPhase();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SelectCharacterWidgetClass;

	UUserWidget* SelectCharacterWidget = nullptr;
	
	UFUNCTION(Client, Reliable)
	void PlayerStateInfoReady();

	UFUNCTION(Client, Reliable)
	void UpdatePlayerWidget();

	// 클라 컨트롤러가 서버 컨트롤러에게 준비 됐다고 보고
	UFUNCTION(Server, Reliable)
	void Server_ControllerIsReady();

	// 모든 플레이어가 캐릭터를 선택하지 않아 로비로 복귀
	UFUNCTION(Client, Reliable)
	void BackToLobby();

	// 인게임 맵 전환 로딩창
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ToInGameLoadingWidgetClass;

	UUserWidget* LoadingWidget;

	UFUNCTION(Client, Reliable)
	void ToInGameLoading();


	
	// ---------------------------------------------
	// 인게임 플레이 페이즈
	// ---------------------------------------------
	void CheckLoadedAllStreamingLevels();
	
	void StartInGamePhase();

	UFUNCTION(BlueprintNativeEvent)
	void BP_StartInGamePhase();

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
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "UI")
	int32 CountdownTime;
	
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
	void HiddenRecallWidget(bool IsRecallCompleted);
	UFUNCTION(Server, Reliable)
	void Server_CancelRecall();
	void CancelRecall();
	void CompleteRecall();
	void RecallToBase();
	UFUNCTION(Client, Reliable)
	void SetClientControlRotation(FRotator ControlRot);

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
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerReady(); // 서버에 준비 완료 신호 전송

public:
	virtual void OnPossess(APawn* InPawn) override;
};
