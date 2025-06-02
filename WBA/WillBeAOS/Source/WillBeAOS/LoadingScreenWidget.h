#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"


UCLASS()
class WILLBEAOS_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite)
	int32 SessionMaxPlayers;
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentSessionPlayers;

public:
	void UpdateSessionMaxPlayers(int32 MaxPlayersNum);
	void UpdateSessionPlayersNum(int32 CurrentPlayersNum);
};
