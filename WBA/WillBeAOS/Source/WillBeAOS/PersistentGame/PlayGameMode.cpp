#include "PersistentGame/PlayGameMode.h"

#include "PlayGameState.h"
#include "Kismet/GameplayStatics.h"


void APlayGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!CharacterSelectStream.IsNull())
	{
		FLatentActionInfo LoadSelectCharLevelLatentInfo;
		LoadSelectCharLevelLatentInfo.CallbackTarget = this;
		LoadSelectCharLevelLatentInfo.ExecutionFunction = FName("SelectLevelOnLoaded");
		LoadSelectCharLevelLatentInfo.Linkage = 0;
		LoadSelectCharLevelLatentInfo.UUID = __LINE__;

		FName SelectCharLevelName = FName(*CharacterSelectStream.GetAssetName());
		UGameplayStatics::LoadStreamLevel(this, SelectCharLevelName, true, false, LoadSelectCharLevelLatentInfo);
	}
}

void APlayGameMode::SelectLevelOnLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetName());
	
	GS = GetWorld()->GetGameState<APlayGameState>();
	if (GS)
	{
		GS->SetGamePhase(EGamePhase::CharacterSelect);
	}
}

void APlayGameMode::StartLoading()
{
	if (GS)
	{
		GS->SetGamePhase(EGamePhase::LoadingPhase);
	}
	
	FLatentActionInfo UnloadSelectCharLevelLatentInfo;
	UnloadSelectCharLevelLatentInfo.CallbackTarget = this;
	UnloadSelectCharLevelLatentInfo.Linkage = 0;
	UnloadSelectCharLevelLatentInfo.UUID = __LINE__;

	FName SelectCharLevelName = FName(*CharacterSelectStream.GetAssetName());
	UGameplayStatics::UnloadStreamLevel(this, SelectCharLevelName, UnloadSelectCharLevelLatentInfo, false);
}

void APlayGameMode::StartSequentialLevelStreaming()
{
	if (StreamingLevelSequence.IsValidIndex(CurrentStreamingLevelIndex))
	{
		FName LevelName = FName(*StreamingLevelSequence[CurrentStreamingLevelIndex].GetAssetName());

		FLatentActionInfo SequenceLatentInfo;
		SequenceLatentInfo.CallbackTarget = this;
		SequenceLatentInfo.ExecutionFunction = FName("OnSequenceLevelLoaded");
		SequenceLatentInfo.Linkage = 0;
		SequenceLatentInfo.UUID = __LINE__;

		UGameplayStatics::LoadStreamLevel(this, LevelName, true, false, SequenceLatentInfo);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("모든 스트리밍 레벨 로드 완료!"));
		
		if (GS)
		{
			GS->SetGamePhase(EGamePhase::InGame);
		}
	}
}

void APlayGameMode::OnSequenceLevelLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("%s 로드 완료"), *StreamingLevelSequence[CurrentStreamingLevelIndex]->GetName());

	CurrentStreamingLevelIndex++;
	StartSequentialLevelStreaming(); // 다음 레벨 로드
}
