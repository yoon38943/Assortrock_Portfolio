#include "Char_Wraith.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void AChar_Wraith::WraithAttack_Implementation(FVector EnemyLocation)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add((UEngineTypes::ConvertToObjectType(ECC_Pawn)));
	ObjectTypes.Add((UEngineTypes::ConvertToObjectType(ECC_WorldStatic)));

	TArray<AActor*> IgnoreActors;
	FHitResult HitResult;

	FVector Start = GetMesh()->GetSocketLocation(FName(TEXT("Muzzle_01")));
	FVector End = ((EnemyLocation - Start) * 0.001f) + EnemyLocation ;

	bool TraceSuccess = UKismetSystemLibrary::LineTraceSingleForObjects(
	this,
	Start,
	End,
	ObjectTypes,
	false,
	IgnoreActors,
	EDrawDebugTrace::ForDuration,
	HitResult,
	true
	);

	if (TraceSuccess)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticle,
			HitResult.Location,
			FRotator::ZeroRotator,
			FVector(0.7f),
			true
			);

		HandleApplyPointDamage(HitResult);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,                     // Key: -1이면 새 메시지
				2.0f,                   // 지속 시간 (초)
				FColor::Green,          // 글자 색
				HasAuthority() ? TEXT("Attack on Server!") : TEXT("Attack on Client!")
			);
		}
	}
}
