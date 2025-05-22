// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WEnemyDetectorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDetected, AActor*, DetectedEnemy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WILLBEAOS_API UWEnemyDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWEnemyDetectorComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Detection")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, Category = "Detection")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(BlueprintAssignable, Category = "Detection")
	FOnEnemyDetected OnEnemyDetected;

	UPROPERTY()
	TArray<AActor*> CurrentlyDetected;

private:
	FTimerHandle DetectionTimer;

	void DetectEnemies();
};
