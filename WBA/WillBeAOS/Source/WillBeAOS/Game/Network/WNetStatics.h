#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "WNetStatics.generated.h"

UCLASS()
class WILLBEAOS_API UWNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port);
	static IOnlineSessionPtr GetSessionPtr();
	static IOnlineIdentityPtr GetIdentityPtr(const UObject* WorldContext);

	static bool IsSessionServer(const UObject* WorldContextObject);
	
	static FName GetPortKey();
	
	static FString GetCommandLineArgAsString(const FName& ParamName);
	static int GetCommandLineArgAsInt(const FName& ParamName);

	static FString GetTestingURL();
	static FName GetTestingURLKey();

	static void ReplacePort(FString& OutURLStr, int NewPort);
};
