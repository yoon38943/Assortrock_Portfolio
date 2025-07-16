#pragma once

#include "CoreMinimal.h"
#include "WCharacterBase.h"
#include "Char_Shinbi.generated.h"

UCLASS()
class WILLBEAOS_API AChar_Shinbi : public AWCharacterBase
{
	GENERATED_BODY()

public:
	AChar_Shinbi();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	TArray<AActor*> GetTartgetInCenter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
