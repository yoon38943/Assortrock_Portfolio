#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubSystem.h"
#include "OnlineSessionSettings.h"
#include "WNetStatics.generated.h"

UCLASS()
class WILLBEAOS_API UWNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port);
	static IOnlineSessionPtr GetSessionPtr();
	static IOnlineIdentityPtr GetIdentityPtr();
	
	static uint8 GetPlayerCountPerTeam();

	static bool IsSessionServer(const UObject* WorldContextObject);

	static FString GetSessionNameStr();
	static FName GetSessionNameKey();
	
	static FString GetSessionSearchIdStr();
	static FName GetSessionSearchIdKey();
	
	static int GetSessionPort();
	static FName GetPortKey();

	static FName GetCoordinatorURLKey();
	static FString GetCoordinatorURL();
	static FString GetDefaultCoordinatorURL();
	
	static FString GetCommandLineArgAsString(const FName& ParamName);
	static int GetCommandLineArgAsInt(const FName& ParamName);
};
