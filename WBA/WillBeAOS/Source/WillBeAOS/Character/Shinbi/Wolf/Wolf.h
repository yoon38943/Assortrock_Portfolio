#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Game/WGameInstance.h"
#include "GameFramework/Pawn.h"
#include "Wolf.generated.h"

UCLASS()
class WILLBEAOS_API AWolf : public APawn
{
	GENERATED_BODY()

public:
	AWolf();
	virtual void Tick(float DeltaTime) override;

	E_TeamID TeamID;

	TArray<AActor*> HitActors;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
					bool bFromSweep, const FHitResult& SweepResult);

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	UBoxComponent* RootCollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	UBoxComponent* CollisionComponent;
};
