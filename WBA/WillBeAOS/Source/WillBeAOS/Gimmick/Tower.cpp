#include "Tower.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "Projectile.h"
#include "../Character/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "../Minions/HealthBar.h"
#include "../Game/WGameState.h"
#include "Character/WPlayerController.h"
#include "Game/WGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATower::ATower()
{
	bReplicates = true;           // 이 액터가 복제되도록 설정
	
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

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

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	WidgetComponent->SetupAttachment(GetRootComponent());
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 200.f));
	WidgetComponent->SetIsReplicated(false);
	WidgetComponent->SetVisibility(false);
	
	// 특정 요소의 오버랩 함수 바인드하기 ( OverlapTrigger의 )
	OverlapTrigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	OverlapTrigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	bAlwaysRelevant = true;

	SetGoldReward(GOLDAMOUNT);
}

void ATower::InitHPPercentage(float Health, float MaxHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("%f, %f"), CombatComp->Health, CombatComp->Max_Health);
	auto Widget = Cast<UHealthBar>(WidgetComponent->GetWidget());

	if (Widget != nullptr)
	{
		if (MaxHealth != 0)
			Widget->HealthBar->SetPercent(Health / MaxHealth);
	}
}

void ATower::S_InitHPPercentage_Implementation()
{
	if (CombatComp->Health != 0)
	{
		InitHPPercentage(CombatComp->Health, CombatComp->Max_Health);
	}
	else
	{
		FTimerHandle HPPercentage;
		GetWorld()->GetTimerManager().SetTimer(HPPercentage, this, &ThisClass::S_InitHPPercentage, 0.1f, false);
	}
}

void ATower::C_InitHPPercentage_Implementation(float Health, float MaxHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("%f, %f"), CombatComp->Health, CombatComp->Max_Health);
	auto Widget = Cast<UHealthBar>(WidgetComponent->GetWidget());

	if (Widget != nullptr)
	{
		if (MaxHealth != 0)
			Widget->HealthBar->SetPercent(Health / MaxHealth);
	}
}

void ATower::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		S_InitHPPercentage();
		InitHPPercentage(CombatComp->Health, CombatComp->Max_Health);
	}

	// 플레이어 컨트롤러 찾기
	FindPlayerPC();

	//찾은 컨트롤러 바탕으로 플레이어 캐릭터와 타워 거리 체크
	StartCheckDistanceToPlayer();
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() && OverlappingActors.IsValidIndex(0))
	{
		TargetOfActors = OverlappingActors[0];

		// 타겟에 빔 조준
		if (TargetOfActors)
			BeamToTarget(TargetOfActors->GetActorLocation(), TargetOfActors);
	}
	
	if (HasAuthority() && OverlappingActors.IsValidIndex(0))
	{
		TargetOfActors = OverlappingActors[0];
		
		// 공격 2초마다 한번씩 스폰
		Delta += DeltaTime;
		if(Delta >= 2)
		{
			Delta = 0;
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;

			FVector SpawnLocation = AttackStartPoint->GetComponentLocation();
			FRotator SpawnRotation = (TargetOfActors->GetActorLocation() - SpawnLocation).Rotation();
			
			AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(SpawnActors, SpawnLocation, SpawnRotation, SpawnParams);

			if (Projectile)
			{
				Projectile->SetHomingTarget();
			}
		}
	}
}

void ATower::BeamToTarget(FVector TargetLocation, AAOSCharacter* Target)
{
	if (Cast<AWCharacterBase>(Target))
	{
		NiagaraComponent->SetVariableLinearColor("BeamColor", FLinearColor::Red);
	}
	else
	{
		NiagaraComponent->SetVariableLinearColor("BeamColor", FLinearColor::Blue);
	}
	
	FVector BeamStart = AttackStartPoint->GetComponentLocation(); // 빔 시작 위치
	FVector BeamEnd = TargetLocation;         // 빔 끝 위치
		
	NiagaraComponent->SetVectorParameter("MyBeamStart", BeamStart);
	NiagaraComponent->SetVectorParameter("MyBeamEnd", BeamEnd);
	NiagaraComponent->SetVisibility(true);
}

void ATower::CheckDistanceToPlayer()
{
	FindPlayerPawn();
	
	if (!PlayerChar) return;

	float Distance = FVector::Dist(PlayerChar->GetActorLocation(), GetActorLocation());
	bool bIsVisible = Distance <= MaxVisibleDistance;

	if (WidgetComponent->IsVisible() != bIsVisible)
	{
		Server_UpdateHPBar();
		WidgetComponent->SetVisibility(bIsVisible);
	}
}

void ATower::StartCheckDistanceToPlayer()
{
	if (!HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
			CheckDistanceTimer,
			this,
			&ATower::CheckDistanceToPlayer,
			0.3f,
			true
			);
	}
}

void ATower::FindPlayerPC()
{
	if (HasAuthority()) return;
	
	PlayerController = Cast<AWPlayerController>(GetWorld()->GetFirstPlayerController());
	FTimerHandle TowerPCTimerManager;
	if (!PlayerController)
	{
		GetWorldTimerManager().SetTimer(TowerPCTimerManager, this, &ThisClass::FindPlayerPC, 0.2f, true);
	}
	else
	{
		if (TowerPCTimerManager.IsValid())
			GetWorldTimerManager().ClearTimer(TowerPCTimerManager);
	}
}

void ATower::FindPlayerPawn()
{
	if (HasAuthority()) return;
	
	if (PlayerController)
	{
		PlayerChar = Cast<AWCharacterBase>(PlayerController->GetPawn());
	}
}

float ATower::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!HasAuthority()) return false;

	LastHitBy = EventInstigator;
	
	float TakeDamage = DamageAmount;
	if (CombatComp != nullptr)
	{
		CombatComp->HandleTakeDamage(TakeDamage);

		S_SetHpPercentage((CombatComp->Health), (CombatComp->Max_Health));

		// 타워의 HP가 일정 이하로 떨어지면 데미지 받은 메쉬로 바꾸고 파티클 생성
		if (CombatComp->Health <= (CombatComp->Max_Health / 2) && !IsParticleSpawned)
		{
			S_SetDamaged();
			
			IsParticleSpawned = true;

			AWGameMode* GameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->OnObjectKilled(this, LastHitBy);
			}
		}

		if ((CombatComp->GetIsDead()))
		{
			AWGameMode* GameMode = Cast<AWGameMode>(GetWorld()->GetAuthGameMode());
			if (GameMode)
			{
				GameMode->OnObjectKilled(this, LastHitBy);
			}
			
			TowerDestroyInClient();
		}
	}

	return DamageAmount;
}

void ATower::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	AAOSCharacter* PlayChar = Cast<AAOSCharacter>(OtherActor);
	if ((PlayChar && PlayChar->TeamID != TeamID))
	{
		// 적군 타겟 배열 등록
		OverlappingActors.AddUnique(PlayChar);
	}
	else if (PlayChar && PlayChar->TeamID == TeamID)
	{
		// 아군일 때 로직
		PlayChar->TowerWithCharacterInside = this;
	}
}

void ATower::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAOSCharacter* DetectedChar = Cast<AAOSCharacter>(OtherActor);
	if (DetectedChar && OverlappingActors.Contains(DetectedChar))
		OverlappingActors.Remove(DetectedChar);
	else if (DetectedChar && DetectedChar->TeamID == TeamID && DetectedChar->TowerWithCharacterInside == this)
		DetectedChar->TowerWithCharacterInside = nullptr;
	
	// 타깃 배열이 비어있으면 스폰 시간 초기화 및 Niagara 비활성화
	if (OverlappingActors.IsEmpty())
	{
		Delta = 0;
		NiagaraComponent->SetVisibility(false);
	}
}

void ATower::TowerDestroyInClient_Implementation()
{
	if (IsValid(CapsuleCollisionComponet))
	{
		FVector DestroyLocation = CapsuleCollisionComponet->GetComponentLocation();
		
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DestroyParticle,
			DestroyLocation,
			FRotator::ZeroRotator,
			FVector(2.f),
			true);
	}

	Destroy();
}

void ATower::Server_UpdateHPBar_Implementation()
{
	float HPPercent = CombatComp->Health / CombatComp->Max_Health;
	Client_UpdateWidget(HPPercent);
}

void ATower::Client_UpdateWidget_Implementation(float HPPercent)
{
	if (UHealthBar* HPBar = Cast<UHealthBar>(WidgetComponent->GetWidget()))
	{
		HPBar->HealthBar->SetPercent(HPPercent);
	}
}

void ATower::spawn()
{
	FActorSpawnParameters SpawnParams;
	GetWorld()->SpawnActor<AActor>(SpawnActors, AttackStartPoint->GetComponentTransform(), SpawnParams);
}

void ATower::DamagedParticle_Implementation()
{
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

void ATower::S_SetHPbarColor_Implementation()
{
	static FLinearColor HealthBarColor;
	switch (TeamID)
	{
	case E_TeamID::Red:
		HealthBarColor = RedTeamColor;
		break;
	case E_TeamID::Blue:
		HealthBarColor = BlueTeamColor;
		break;
	case E_TeamID::Neutral:
		HealthBarColor = DefaultColor;
		break;
	}

	SetHPbarColor(HealthBarColor);
}

void ATower::SetHPbarColor_Implementation(FLinearColor HealthBarColor)
{
	UHealthBar* HealthBarWidget = Cast<UHealthBar>(WidgetComponent->GetWidget());
	if (!HealthBarWidget)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, HealthBarColor]()
		{
			SetHPbarColor(HealthBarColor);
		});
		return;
	}
	
	if (HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetFillColorAndOpacity(HealthBarColor);

		HealthBarWidget->InvalidateLayoutAndVolatility();
	}
}

void ATower::S_SetDamaged_Implementation()
{
	NM_SetDamaged();
}

void ATower::NM_SetDamaged_Implementation()
{
	StaticMesh->SetStaticMesh(DamagedStaticMesh);
	DamagedParticle();
}

void ATower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(ATower, TargetOfActors);
	DOREPLIFETIME(ATower, OverlappingActors);
}