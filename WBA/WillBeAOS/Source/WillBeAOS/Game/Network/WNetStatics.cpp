#include "Game/Network/WNetStatics.h"

FOnlineSessionSettings UWNetStatics::GenerateOnlineSessionSettings(const FName& SessionName,
	const FString& SessionSearchId, int Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false;
	OnlineSessionSettings.NumPublicConnections = 1;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = true;
	OnlineSessionSettings.bAllowJoinViaPresence = true;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = false;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;

	OnlineSessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	return OnlineSessionSettings;
}

IOnlineSessionPtr UWNetStatics::GetSessionPtr()
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UWNetStatics::GetIdentityPtr()
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetIdentityInterface();
	}
	return nullptr;
}

uint8 UWNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UWNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == NM_DedicatedServer;
}

FString UWNetStatics::GetSessionNameStr()
{
	return GetCommandLineArgAsString(GetSessionNameKey());
}

FName UWNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UWNetStatics::GetSessionSearchIdStr()
{
	return GetCommandLineArgAsString(GetSessionSearchIdKey());
}

FName UWNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UWNetStatics::GetSessionPort()
{
	int Port = GetCommandLineArgAsInt(GetPortKey());

	if (Port == 0)
	{
		return 7777;
	}
	
	return Port;
}

FName UWNetStatics::GetPortKey()
{
	return FName("PORT_NAME");
}

FName UWNetStatics::GetCoordinatorURLKey()
{
	return FName("COORDINATOR_URL");
}

FString UWNetStatics::GetCoordinatorURL()
{
	FString CoordinatorURL = GetCommandLineArgAsString(GetCoordinatorURLKey());
	if (CoordinatorURL == "")
	{
		return CoordinatorURL;
	}

	return GetDefaultCoordinatorURL();
}

FString UWNetStatics::GetDefaultCoordinatorURL()
{
	FString CoordinatorURL = "";
	GConfig->GetString(TEXT("WillBeAOS.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	UE_LOG(LogTemp, Warning, TEXT("CoordinatorURL: %s"), *CoordinatorURL);
	return CoordinatorURL;
}

FString UWNetStatics::GetCommandLineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UWNetStatics::GetCommandLineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}
