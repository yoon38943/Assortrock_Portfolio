#include "Tower.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "../Character/WCharacterBase.h"
#include "../Character/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "../Minions/HealthBar.h"
#include "../Game/WGameState.h"
#include "../Minions/WMinionsCharacterBase.h"
#include "Game/WGameMode.h"
#include "Net/UnrealNetwork.h"

ATower::ATower()
{
	bReplicates = true;           // 이 액터가 복제되도록 설정
	
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	DamagedNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DamagedParticle"));
	DamagedNiagara->SetupAttachment(GetRootComponent());

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraParticleSystem"));
	NiagaraComponent->SetupAttachment(GetRootComponent());

	OverlapTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapTrigger"));
	OverlapTrigger->SetupAttachment(GetRootComponent());

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());

	CapsuleCollisionComponet = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollisionComponet->SetupAttachment(GetRootComponent());

	AttackStartPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackStartPoint"));
	AttackStartPoint->SetupAttachment(GetRootComponent());

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.f));

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	// 특정 요소의 오버랩 함수 바인드하기 ( OverlapTrigger의 )
	OverlapTrigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	OverlapTrigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
	
}

int32 ATower::GetGoldReward() const
{
	return GoldReward;
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
}



void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OverlappingActors.IsValidIndex(0))
	{

		TargetOfActors = OverlappingActors[0];


		FVector BeamStart = AttackStartPoint->GetComponentLocation(); // 빔 시작 위치
		FVector BeamEnd = TargetOfActors->GetActorLocation();         // 빔 끝 위치
		
		NiagaraComponent->SetVectorParameter("MyBeamStart", BeamStart);
		NiagaraComponent->SetVectorParameter("MyBeamEnd", BeamEnd);
		NiagaraComponent->SetVisibility(true);

		
		// 공격 2초마다 한번씩 스폰
		Delta += DeltaTime;
		if(Delta >= 2)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			GetWorld()->SpawnActor<AActor>(SpawnActors, AttackStartPoint->GetComponentTransform(), SpawnParams);
			Delta = 0;
		}
	}
}

void ATower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, TowerTeamID);
}


float ATower::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	LastHitBy = EventInstigator;
	
	float TakeDamage = DamageAmount;
	if (CombatComp != nullptr)
	{
		CombatComp->HandleTakeDamage(TakeDamage);

		S_SetHpPercentage((CombatComp->Health), (CombatComp->Max_Health));

		// 타워의 HP가 일정 이하로 떨어지면 데미지 받은 메쉬로 바꾸고 파티클 생성
		if (CombatComp->Health <= (CombatComp->Max_Health / 2) && !IsParticleSpawned)
		{
			StaticMesh->SetStaticMesh(DamagedStaticMesh);
			DamagedNiagara->SetAsset(DamageParticle);
			IsParticleSpawned = true;

			AWGameMode* GameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->OnObjectKilled(this, LastHitBy);
			}
		}

		if ((CombatComp->GetIsDead()))
		{
			AWGS = Cast<AWGameState>(GetWorld()->GetGameState());
			//DefaultSceneRoot->SetVisibility(false, true);
			if (AWGS != nullptr)
			{
				AWGS->TowerArray.Remove(this);
			}

			AWGameMode* GameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->OnObjectKilled(this, LastHitBy);
			}
			
			Destroy();
		}
	}

	return DamageAmount;
}

void ATower::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWCharacterBase* EnemyChar = Cast<AWCharacterBase>(OtherActor);
	AWMinionsCharacterBase* EnemyMinion = Cast<AWMinionsCharacterBase>(OtherActor);
	if ((EnemyChar && EnemyChar->TeamID != TowerTeamID) || EnemyMinion)
	{
		OverlappingActors.AddUnique(OtherActor);
	}
}

void ATower::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingActors.Remove(OtherActor);


	// 타깃 배열이 비어있으면 스폰 시간 초기화 및 Niagara 비활성화
	if (OverlappingActors.IsEmpty())
	{
		Delta = 0;
		NiagaraComponent->SetVisibility(false);
	}
}

void ATower::spawn()
{
	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<AActor>(SpawnActors, AttackStartPoint->GetComponentTransform(), SpawnParams);
}

void ATower::S_SetHpPercentage_Implementation(float Health, float MaxHealth)
{
	SetHpPercentage(Health, MaxHealth);
}

void ATower::SetHpPercentage_Implementation(float Health, float MaxHealth)
{
	auto Widget = Cast<UHealthBar>(WidgetComponent->GetWidget());

	if (Widget != nullptr)
	{
		if (MaxHealth != 0)
			Widget->HealthBar->SetPercent(Health / MaxHealth);
	}
}
