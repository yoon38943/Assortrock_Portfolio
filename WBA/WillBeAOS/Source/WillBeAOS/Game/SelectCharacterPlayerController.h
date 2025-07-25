#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SelectCharacterPlayerController.generated.h"

UCLASS()
class WILLBEAOS_API ASelectCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SelectCharacterWidgetClass;

public:
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

	UFUNCTION(Client, Reliable)
	void ToInGameLoading();
};
