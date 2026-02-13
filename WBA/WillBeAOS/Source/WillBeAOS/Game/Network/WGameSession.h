#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "WGameSession.generated.h"

UCLASS()
class WILLBEAOS_API AWGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	virtual bool ProcessAutoLogin() override;

	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
	virtual void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override;
};
