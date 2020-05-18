// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIModule/Classes/AIController.h"
#include "../Player/MainPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "UMG/Public/Components/ProgressBar.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//ChaseVolume
	this->ChaseVolume = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseVolume"));
	this->ChaseVolume->SetupAttachment(GetRootComponent());
	this->ChaseVolume->SetSphereRadius(800.0f);
	this->ChaseVolume->SetCollisionObjectType(ECC_WorldDynamic);
	this->ChaseVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->ChaseVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	//AttackVolume
	this->AttackVolume = CreateDefaultSubobject<USphereComponent>(TEXT("AttackVolume"));
	this->AttackVolume->SetupAttachment(GetRootComponent());
	this->AttackVolume->SetSphereRadius(100.0f);
	this->AttackVolume->SetCollisionObjectType(ECC_WorldDynamic);
	this->AttackVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->AttackVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	this->HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	this->HealthBarWidgetComponent->SetupAttachment(GetRootComponent());
	this->HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	this->HealthBarWidgetComponent->SetDrawSize(FVector2D(125.0f, 10.0f));
	//this->HealthBarWidgetComponent->SetWidgetClass()

	this->LeftAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftAttackCollision"));
	this->LeftAttackCollision->SetupAttachment(this->GetMesh(), FName("LeftAttackSocket"));
	this->DeactiveLeftAttackCollision();

	this->RightAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightAttackCollision"));
	this->RightAttackCollision->SetupAttachment(this->GetMesh(), FName("RightAttackSocket"));
	this->DeactiveRightAttackCollision();


	//防止玩家角色的SpringArm受到阻挡影响
	this->GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	this->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//确定Pawn何时创建并由AI Controller拥有（在关卡开始时，生成时等）。
	this->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	
	this->bAttackVolumeOverlapping = false;

	this->InterpSpeed = 15.0f;
	this->bInterpToPlayer = false;

	this->EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
	this->MaxHealth = 100.0f;
	this->Health = this->MaxHealth;
	
	this->Damage = 10.0f;

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	this->ChaseVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnChaseVolumeOverlapBegin);
	this->ChaseVolume->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnChaseVolumeOverlapEnd);

	this->AttackVolume->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnAttackVolumeOverlapBegin);
	this->AttackVolume->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnAttackVolumeOverlapEnd);

	////从生成的UI实例中获取ProgressBar
	HealthBar = Cast<UProgressBar>(HealthBarWidgetComponent->GetUserWidgetObject()->GetWidgetFromName("HealthBar"));
	this->HealthBar->SetPercent(Health / MaxHealth);
	this->HealthBar->SetVisibility(ESlateVisibility::Hidden);
	
	this->LeftAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnLeftAttackCollisionOverlapBegin);
	this->LeftAttackCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnLeftAttackCollisionOverlapEnd);

	this->RightAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnRightAttackCollisionOverlapBegin);
	this->RightAttackCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnRightAttackCollisionOverlapEnd);

	//获取AIController的引用
	this->AIController = Cast<AAIController>(this->GetController());
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (this->bInterpToPlayer&&HasValidTarget()&&IsAlive())
	{
		float LookAtYaw = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation()).Yaw;
		FRotator LookAtRotation(0.0f, LookAtYaw, 0.0f);
		FRotator InterpRotation = FMath::RInterpTo(this->GetActorRotation(), LookAtRotation, DeltaTime, this->InterpSpeed);
		this->SetActorRotation(InterpRotation);
	}

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::OnChaseVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasValidTarget()||!IsAlive())
	{
		return;
	}
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			this->HealthBar->SetVisibility(ESlateVisibility::Visible);
			MoveToTarget(MainPlayer);
		}
	}
}

void ABaseEnemy::OnChaseVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			this->HealthBar->SetVisibility(ESlateVisibility::Hidden);
			this->EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;

			if (AIController)
			{
				this->AIController->StopMovement();
			}
		}
	}
}

void ABaseEnemy::OnAttackVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			MainPlayer->UpdateAttackTarget();
			this->bAttackVolumeOverlapping = true;
			Attack();
		}
	}
}

void ABaseEnemy::OnAttackVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			this->bAttackVolumeOverlapping = false;
			if (this->EnemyMovementStatus != EEnemyMovementStatus::EEMS_Attacking)
			{
				MoveToTarget(MainPlayer);
			}
		}
	}
}

void ABaseEnemy::MoveToTarget(AMainPlayer* TargetPlayer)
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	this->EnemyMovementStatus = EEnemyMovementStatus::EEMS_MoveToTarget;

	if (this->AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(TargetPlayer);
		//寻路停止半径;
		MoveRequest.SetAcceptanceRadius(10.0f);
		FNavPathSharedPtr NavPath;
		
		this->AIController->MoveTo(MoveRequest,OUT &NavPath);

		//场景中绘制圆形
		//auto PathPoints = NavPath->GetPathPoints();
		//for (auto Point : PathPoints)
		//{
		//	FVector Location = Point.Location;
		//	UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.0f, 8, FLinearColor::Red, 10.0f, 1.5f);
		//}

	}
}

void ABaseEnemy::Attack()
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	if (this->AIController)
	{
		AIController->StopMovement();
	}
	if (this->EnemyMovementStatus != EEnemyMovementStatus::EEMS_Attacking)
	{
		this->EnemyMovementStatus = EEnemyMovementStatus::EEMS_Attacking;
		this->bInterpToPlayer = true;

		UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();
		if (AnimInstance&&this->AttackMontage)
		{
			const float PlayRate = FMath::RandRange(0.9f, 1.1f);
			FString SectionName = FString::FromInt(FMath::RandRange(1, 3));
			AnimInstance->Montage_Play(this->AttackMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(FName(*SectionName), this->AttackMontage);
		}
		
	}
}

void ABaseEnemy::AttackEnd()
{
	this->bInterpToPlayer = false;
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	this->EnemyMovementStatus = EEnemyMovementStatus::EEMS_Idle;
	if (this->bAttackVolumeOverlapping)
	{
		Attack();
	}
	
}

void ABaseEnemy::OnLeftAttackCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			if (MainPlayer->HitParticles)
			{
				const USkeletalMeshSocket* AttackSocket = GetMesh()->GetSocketByName("LeftAttackSocket");
				if (AttackSocket)
				{
					FVector SocketLocation = AttackSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(this, MainPlayer->HitParticles, SocketLocation, FRotator(0.0f), true);
				}
			}
			if (MainPlayer->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainPlayer->HitSound);
			}
			if (this->DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainPlayer, this->Damage / 2.0f, this->AIController, this, this->DamageTypeClass);
			}
		}
	}
}

void ABaseEnemy::OnLeftAttackCollisionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABaseEnemy::OnRightAttackCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasValidTarget() || !IsAlive())
	{
		return;
	}
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			if (MainPlayer->HitParticles)
			{
				const USkeletalMeshSocket* AttackSocket = GetMesh()->GetSocketByName("RightAttackSocket");
				if (AttackSocket)
				{
					FVector SocketLocation = AttackSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(this, MainPlayer->HitParticles, SocketLocation, FRotator(0.0f), true);
				}
			}
			if (MainPlayer->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainPlayer->HitSound);
			}
			if (this->DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainPlayer, this->Damage / 2.0f, this->AIController, this, this->DamageTypeClass);
			}
		}
	}
}

void ABaseEnemy::OnRightAttackCollisionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABaseEnemy::ActiveLeftAttackCollision()
{
	this->LeftAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//QueryOnly会影响Movement移动，PhysicalOnly似乎不会影响移动碰撞
	this->LeftAttackCollision->SetCollisionObjectType(ECC_WorldDynamic);
	this->LeftAttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->LeftAttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABaseEnemy::DeactiveLeftAttackCollision()
{
	this->LeftAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseEnemy::ActiveRightAttackCollision()
{
	this->RightAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//QueryOnly会影响Movement移动，PhysicalOnly似乎不会影响移动碰撞
	this->RightAttackCollision->SetCollisionObjectType(ECC_WorldDynamic);
	this->RightAttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->RightAttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABaseEnemy::DeactiveRightAttackCollision()
{
	this->RightAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float ABaseEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (this->Health - Damage <= 0.0f)
	{
		this->Health = FMath::Clamp(this->Health - Damage, 0.0f, this->MaxHealth);
		//TODO 
		Die();
	}
	else
	{
		this->Health -= Damage;
	}
	this->HealthBar->SetPercent(this->Health / this->MaxHealth);

	return this->Health;
}

void ABaseEnemy::Die()
{
	this->EnemyMovementStatus = EEnemyMovementStatus::EEMS_Dead;
	this->HealthBar->SetVisibility(ESlateVisibility::Hidden);

	this->DeactiveLeftAttackCollision();
	this->DeactiveRightAttackCollision();
	this->ChaseVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->AttackVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Cast<AMainPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0))->UpdateAttackTarget();
}

void ABaseEnemy::DeathEnd()
{
	//暂停动画
	this->GetMesh()->bPauseAnims = true;
	//停止骨架更新
	this->GetMesh()->bNoSkeletonUpdate = true;

	FTimerHandle DeathTimerHandle;
	auto Lambda = [this]() {
		//TODO 
		this->Destroy();
	};

	GetWorldTimerManager().SetTimer(DeathTimerHandle, FTimerDelegate::CreateLambda(Lambda), 1.0f, false);
}

bool ABaseEnemy::HasValidTarget()
{
	AMainPlayer* MainPlayer = Cast<AMainPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!MainPlayer)
	{
		return false;
	}
	return MainPlayer->MovementStatus != EPlayerMovementStatus::EPMS_Dead;
}
