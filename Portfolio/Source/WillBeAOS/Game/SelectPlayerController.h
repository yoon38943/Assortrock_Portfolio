#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SelectPlayerController.generated.h"

#define SELECTTIME 10.0f
UCLASS()
class WILLBEAOS_API ASelectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	ASelectPlayerController();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Game")
	class ASelectGameState* SelectGS;
protected:
	virtual void BeginPlay() override;
	
public: //위젯 업데이트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly , Category = "UI")
	TSubclassOf<UUserWidget> SelectWidgetClass;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UUserWidget* SelectWidget;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "value")
	int32 OldWidgetID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "value")
	int32 ActiveWidgetID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "value")
	int32 OwnWidgetID;
public:
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category= "Server")
	void S_ServerUpdateChecked(int32 MWidgetID, int32 MUnCheckWidgetID, const FText& MNickName);
	
	UFUNCTION(BlueprintCallable, category = "UI")
	void SetCharName(int32 MWidgetID, const FText& MCharName);//서버 함수 실행하는 역할
	
	UFUNCTION(Server, Reliable, BlueprintCallable, category = "Server")
	void S_SetCharName(int32 MWidgetID, const FText& MCharName);
	
	UFUNCTION(BlueprintCallable, category = "UI")
	void HandleWidgetUpdate(int32 MWidgetID, const FText& MUserNickName);
	
	UFUNCTION(BlueprintCallable, category = "UI")
	void CancelWidget(int32 MCancelWidget);
	
	UFUNCTION(BlueprintNativeEvent, category = "UI")
	void UpdateTeamSlot(int32 MWidgetID, const FText& MUserNickName);
	
	UFUNCTION(BlueprintNativeEvent, category = "UI")
	void UnCheckTeamSlot(int32 OldWidget);
	
	UFUNCTION(BlueprintNativeEvent, category = "UI")
	void UpdateChar(int32 MWidgetID, const FText& MCharName);
	
public:		//맵 이동
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Game")
	int32 WorldTime = SELECTTIME;
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Server")
	void S_MapTravel();
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Server")
	void S_IsReady(int TeamID, bool Ready, TSubclassOf<APawn> PawnClass);
	
};
