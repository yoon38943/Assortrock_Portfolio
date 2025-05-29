#include "WMinionsCharacterBase.h"
#include "../Character/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "WMinionsAIController.h"
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "HealthBar.h"
#include "../Game/WGameState.h"
#include "Character/WCharacterBase.h"
#include "Character/WPlayerController.h"
#include "Function/WEnemyDetectorComponent.h"
#include "Game/WGameMode.h"
#include "Gimmick/Tower.h"
#include "Net/UnrealNetwork.h"


AWMinionsCharacterBase::AWMinionsCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetCombatEnable(false);
	CombatComponent->SetCollisionMesh(GetMesh());

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.f));
	WidgetComponent->SetVisibility(false);

	EnemyDetector = CreateDefaultSubobject<UWEnemyDetectorComponent>(TEXT("Perception"));
	EnemyDetector->EnemyClass = AActor::StaticClass();
	EnemyDetector->DetectionRadius = 1000.f;
	EnemyDetector->OnEnemyDetected.AddDynamic(this, &ThisClass::HandleEnemyDetected);
	
	SetGoldReward(KILLGOLD);
}

void AWMinionsCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	CombatComponent->DelegateDead.BindUObject(this, &ThisClass::Dead);
	//HandleApplyPointDamage 멀티델리게이트 바인딩
	CombatComponent->DelegatePointDamage.AddUObject(this, &ThisClass::HandleApplyPointDamage);

	FindPlayerPC();

	if (!HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			CheckDistanceTimer,
			this,
			&AWMinionsCharacterBase::CheckDistanceToPlayer,
			0.3f,
			true
			);
	}
}

void AWMinionsCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWMinionsCharacterBase::CheckDistanceToPlayer()
{
	if (bIsDead || !PlayerChar) return;

	float Distance = FVector::Dist(PlayerChar->GetActorLocation(), GetActorLocation());
	bool bIsVisible = Distance <= MaxVisibleDistance;
	
	if (WidgetComponent->IsVisible() != bIsVisible)
	{
		WidgetComponent->SetVisibility(bIsVisible);
	}
}

void AWMinionsCharacterBase::FindPlayerPC()
{
	if (HasAuthority()) return;
	
	PlayerController = Cast<AWPlayerController>(GetWorld()->GetFirstPlayerController());
	FTimerHandle MinionPCTimerManager;
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is null"));
		GetWorldTimerManager().SetTimer(MinionPCTimerManager, this, &ThisClass::FindPlayerPC, 0.2f, true);
	}
	else
	{
		if (MinionPCTimerManager.IsValid())
			GetWorldTimerManager().ClearTimer(MinionPCTimerManager);
	}
	
	FindPlayerPawn();
}

void AWMinionsCharacterBase::FindPlayerPawn()
{
	if (HasAuthority()) return;
	
	if (PlayerController)
	{
		PlayerChar = Cast<AWCharacterBase>(PlayerController->GetPawn());
	}
}

void AWMinionsCharacterBase::S_SetHpPercentage_Implementation(float Health, float MaxHealth)
{
	SetHpPercentage(Health, MaxHealth);
}

void AWMinionsCharacterBase::SetHpPercentage_Implementation(float Health, float MaxHealth)
{
	auto Widget = Cast<UHealthBar>(WidgetComponent->GetWidget());

	if (Widget != nullptr)
	{
		if (MaxHealth != 0)
			Widget->HealthBar->SetPercent(Health / MaxHealth);
	}
}

void AWMinionsCharacterBase::S_SetHPbarColor_Implementation()
{
	static FLinearColor HealthBarColor;
	switch (TeamID)
	{
	case E_TeamID::Red:
		HealthBarColor = FLinearColor::Red;
		break;
	case E_TeamID::Blue:
		HealthBarColor = FLinearColor::Blue;
		break;
	case E_TeamID::Neutral:
		HealthBarColor = FLinearColor::Yellow;
		break;
	}

	SetHPbarColor(HealthBarColor);
}

void AWMinionsCharacterBase::SetHPbarColor_Implementation(FLinearColor HealthBarColor)
{
	if (!WidgetComponent || !WidgetComponent->GetWidget()) {
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, HealthBarColor]()
		{
			SetHPbarColor(HealthBarColor);
		});
		return;
	}

	UHealthBar* Widget = Cast<UHealthBar>(WidgetComponent->GetWidget());
	if (!Widget || !Widget->HealthBar)
	{
		return;
	}
	
	if (Widget->HealthBar)
	{
		Widget->HealthBar->SetFillColorAndOpacity(HealthBarColor);

		Widget->InvalidateLayoutAndVolatility();
	}
}

void AWMinionsCharacterBase::RetrySetHPbarColor(FLinearColor HealthBarColor)
{
	SetHPbarColor(HealthBarColor);
}

void AWMinionsCharacterBase::Dead()
{
	if (!HasAuthority()) return;
	
	AWGameMode* GameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnObjectKilled(this, LastHitBy);
	}
	
	// AI가 죽으면 BT 연결 끊기
	AWMinionsAIController* MinionController = Cast<AWMinionsAIController>(GetController());
	MinionController->GetBrainComponent()->StopLogic(TEXT("None"));

	bIsDead = true;

	GetWorld()->GetTimerManager().ClearTimer(CheckDistanceTimer);

	FTimerHandle MinionDeadTimer;
	GetWorld()->GetTimerManager().SetTimer(MinionDeadTimer, [this]()
	{
		Destroy();
	}, 2.5f, false);
	
	NM_BeingDead();
}

void AWMinionsCharacterBase::NM_BeingDead_Implementation()
{
	//콜리전 없애기
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);

	if (!HasAuthority())
	{
		// 죽는 애니메이션 실행
		PlayAnimMontage(DeadAnimMontage);

		// HP Widget 없애기
		WidgetComponent->SetVisibility(false);
	}
}

void AWMinionsCharacterBase::HandleEnemyDetected(AActor* Enemy)
{
	if (!Enemy) return;

	OnEnemyDetected.Broadcast(Enemy);
}

void AWMinionsCharacterBase::HandleApplyPointDamage(FHitResult LastHit)
{
	if (HasAuthority())
	{
		// ----- 같은팀 캐릭터, 미니언 타격 무효 -----
		AAOSCharacter* HitCharacter = Cast<AAOSCharacter>(LastHit.GetActor());
		if (HitCharacter)
		{
			if (this->TeamID == HitCharacter->TeamID) return;
		}
		// ----- 같은팀 타워, 넥서스 타격 무효 -----
		AAOSActor* HitObject = Cast<AAOSActor>(LastHit.GetActor());
		if (HitObject)
		{
			if (this->TeamID == HitObject->TeamID) return;
		}

		// ------ 다른팀 오브젝트 타격시 -----
		UGameplayStatics::ApplyPointDamage(
			LastHit.GetActor(),
			CharacterDamage,
			GetOwner()->GetActorForwardVector(),
			LastHit,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
	}
}

float AWMinionsCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	LastHitBy = EventInstigator;	// 타격 캐릭터 저장
	
	float TakeDamage = DamageAmount;
	CombatComponent->HandleTakeDamage(TakeDamage);

	SetHpPercentage((CombatComponent->Health), (CombatComponent->Max_Health));

	return DamageAmount;
}

void AWMinionsCharacterBase::NM_Minion_Attack_Implementation()
{
}

void AWMinionsCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,TrackNum);
}
