#include "WMinionsAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

void AWMinionsAIController::BeginPlay()
{
	Super::BeginPlay();

	if (MinionBT != nullptr)
	{
		RunBehaviorTree(MinionBT);

		// 플레이어 폰 저장
		//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		//SetFocus(PlayerPawn);
	}
}

void AWMinionsAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}
