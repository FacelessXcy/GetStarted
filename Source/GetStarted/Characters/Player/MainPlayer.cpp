// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "../Enemy/BaseEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMainPlayer::AMainPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->SpringArm=CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	this->SpringArm->SetupAttachment(GetRootComponent());
	this->SpringArm->TargetArmLength=600.0f;
	this->SpringArm->bUsePawnControlRotation=true;
	
	this->FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	this->FollowCamera->SetupAttachment(this->SpringArm,USpringArmComponent::SocketName);
	this->FollowCamera->bUsePawnControlRotation=false;

	GetCapsuleComponent()->SetCapsuleSize(35.0f,100.0f);

	this->bUseControllerRotationPitch=false;
	this->bUseControllerRotationYaw=false;
	this->bUseControllerRotationRoll=false;
	
	//使Character朝向加速方向
	this->GetCharacterMovement()->bOrientRotationToMovement = true;
	this->GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	this->GetCharacterMovement()->JumpZVelocity = 500.0f;
	this->GetCharacterMovement()->AirControl = 0.15f;
	
	this->BaseTurnRate = 65.0f;
	this->BaseLookUpRate = 65.0f;

	this->MaxHealth = 100.0f;
	this->Health = this->MaxHealth;
	this->MaxStamina = 150.0f;
	this->Stamina = this->MaxStamina;
	this->StaminaConsumeRate = 20.0f;
	this->ExhaustedStaminaRatio = 0.167f;
	this->StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;
	this->Coins = 0;

	this->RunningSpeed = 600.0f;
	this->SprintingSpeed = 900.0f;
	this->MovementStatus = EPlayerMovementStatus::EPMS_Normal;
	this->bLeftShiftKeyDown = false;

	this->bAttackKeyDown = false;
	this->bIsAttacking = false;
	this->AttackTarget = nullptr;

	this->InterpSpeed = 15.0f;
	this->bInterpToEnemy = false;
}

// Called when the game starts or when spawned
void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!this->IsAlive())
	{
		return;
	}

	switch (this->StaminaStatus)
	{
	case EPlayerStaminaStatus::EPSS_Normal:
		if (this->bLeftShiftKeyDown)
		{//按住Shift
			if (this->Stamina - this->StaminaConsumeRate * DeltaTime <= this->MaxStamina * this->ExhaustedStaminaRatio)
			{//这帧之后，进入疲劳区，改变状态
				this->StaminaStatus = EPlayerStaminaStatus::EPSS_Exhausted;
			}
			//更新这一帧状态
			this->Stamina -= this->StaminaConsumeRate * DeltaTime;
			SetMovementStatus(EPlayerMovementStatus::EPMS_Sprinting);
		}
		else
		{//没有按Shift，更新移动状态
			this->Stamina = FMath::Clamp(this->Stamina + this->StaminaConsumeRate * DeltaTime, 0.0f, this->MaxStamina);
			SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
		}
		break;
	case EPlayerStaminaStatus::EPSS_Exhausted:
		if (this->bLeftShiftKeyDown)
		{
			if (this->Stamina - this->StaminaConsumeRate * DeltaTime <= 0.0f)
			{//耐力用完，自动抬起Shift，更新移动状态和耐力状态
				LeftShiftKeyUp();
				this->StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
				this->SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
			}
			else
			{
				this->Stamina -= this->StaminaConsumeRate * DeltaTime;
				//SetMovementStatus(EPlayerMovementStatus::EPMS_Sprinting);
			}
		}
		else
		{//抬起按键,回复能量
			this->StaminaStatus = EPlayerStaminaStatus::EPSS_ExhaustedRecovering;
			this->Stamina = FMath::Clamp(this->Stamina + this->StaminaConsumeRate * DeltaTime, 0.0f, this->MaxStamina);
			SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
		}
		break;
	case EPlayerStaminaStatus::EPSS_ExhaustedRecovering:
		if (this->Stamina + this->StaminaConsumeRate * DeltaTime >= this->MaxStamina * this->ExhaustedStaminaRatio)
		{
			this->StaminaStatus = EPlayerStaminaStatus::EPSS_Normal;
		}
		this->Stamina += this->StaminaConsumeRate * DeltaTime;
		LeftShiftKeyUp();
		SetMovementStatus(EPlayerMovementStatus::EPMS_Normal);
		break;
	default:
		break;
	}

	//插值，朝向攻击目标
	if (this->bInterpToEnemy&&AttackTarget)
	{
		float LookAtYaw = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), AttackTarget->GetActorLocation()).Yaw;
		FRotator LookAtRotation(0.0f, LookAtYaw, 0.0f);
		FRotator InterpRotation = FMath::RInterpTo(this->GetActorRotation(), LookAtRotation, DeltaTime, this->InterpSpeed);
		this->SetActorRotation(InterpRotation);
	}

}

void AMainPlayer::Jump()
{
	if (!this->bIsAttacking&&this->IsAlive())
	{
		Super::Jump();
	}
}

// Called to bind functionality to input
void AMainPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Action
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMainPlayer::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AMainPlayer::LeftShiftKeyDown);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AMainPlayer::LeftShiftKeyUp);

	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AMainPlayer::InteractKeyDown);

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AMainPlayer::AttackKeyDown);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &AMainPlayer::AttackKeyUp);


	//Axis
	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this,&AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this,&AMainPlayer::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"),this,&AMainPlayer::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"),this, &AMainPlayer::LookUp);

	PlayerInputComponent->BindAxis(TEXT("TurnAtRate"), this, &AMainPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpAtRate"), this, &AMainPlayer::LookUpAtRate);
	
}


void AMainPlayer::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !this->bIsAttacking && this->IsAlive())
	{
		//获取当前控制器的旋转
		const FRotator Rotation = this->Controller->GetControlRotation();
		//提取出控制器的Z轴的旋转
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
		//根据控制器Z轴方向上的旋转得到这个方向的X轴
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		this->AddMovementInput(Direction, Value);
	}
	
}

void AMainPlayer::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !this->bIsAttacking && this->IsAlive())
	{
		//获取当前控制器的旋转
		const FRotator Rotation = this->Controller->GetControlRotation();
		//提取出控制器的Z轴的旋转
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
		//根据控制器Z轴方向上的旋转得到这个方向的X轴
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		this->AddMovementInput(Direction, Value);
	}
}

void AMainPlayer::Turn(float Value)
{
	if (Value && this->IsAlive())
	{
		AddControllerYawInput(Value);
	}
}

void AMainPlayer::LookUp(float Value)
{
	if (!this->IsAlive())
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("%f"), GetControlRotation().Pitch);
	if (GetControlRotation().Pitch < 270.0f && GetControlRotation().Pitch>180.0f && Value > 0.0f)
	{
		return;
	}
	if (GetControlRotation().Pitch > 45.0f && GetControlRotation().Pitch < 180.0f && Value < 0.0f)
	{
		return;
	}
	AddControllerPitchInput(Value);	
}

void AMainPlayer::TurnAtRate(float Rate)
{
	float Value = Rate * this->BaseTurnRate*GetWorld()->GetDeltaSeconds();
	if (Value && this->IsAlive())
	{
		AddControllerYawInput(Value);
	}
}

void AMainPlayer::LookUpAtRate(float Rate)
{
	if (!this->IsAlive())
	{
		return;
	}
	float Value = Rate * this->BaseLookUpRate * GetWorld()->GetDeltaSeconds();
	if (GetControlRotation().Pitch < 270.0f && GetControlRotation().Pitch>180.0f && Value > 0.0f)
	{
		return;
	}
	if (GetControlRotation().Pitch > 45.0f && GetControlRotation().Pitch < 180.0f && Value < 0.0f)
	{
		return;
	}
	AddControllerPitchInput(Value);
}

void AMainPlayer::IncreaseHealth(float Value)
{
	this->Health = FMath::Clamp(this->Health + Value, 0.0f, this->MaxHealth);
}

void AMainPlayer::IncreaseStamina(float Value)
{
	this->Stamina = FMath::Clamp(this->Stamina + Value, 0.0f, this->MaxStamina);
}

void AMainPlayer::IncreaseCoin(int32 Value)
{
	this->Coins += Value;
}

float AMainPlayer::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (this->Health-Damage<=0.0f)
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, this->MaxHealth);
		//TODO 
		Die();
	}
	else
	{
		Health -= Damage;
	}
	
	return Health;
}

void AMainPlayer::SetMovementStatus(EPlayerMovementStatus Status)
{
	if (!this->IsAlive())
	{
		return;
	}
	this->MovementStatus = Status;
	switch (this->MovementStatus)
	{
		case EPlayerMovementStatus::EPMS_Sprinting:
			this->GetCharacterMovement()->MaxWalkSpeed = this->SprintingSpeed;
			break;
		default:
			this->GetCharacterMovement()->MaxWalkSpeed = this->RunningSpeed;
			break;
	}
}

void AMainPlayer::InteractKeyDown()
{
	if (this->OverlappingWeapon && this->IsAlive())
	{
		if (this->EquippedWeapon)
		{
			this->EquippedWeapon->UnEquip(this);
		}
		this->OverlappingWeapon->Equip(this);
	}
	else
	{
		if (this->EquippedWeapon)
		{
			this->EquippedWeapon->UnEquip(this);
		}
	}

}

void AMainPlayer::AttackKeyDown()
{
	if (!this->IsAlive())
	{
		return;
	}
	this->bAttackKeyDown = true;
	if (this->bHasWeapon)
	{
		Attack();
	}
}

void AMainPlayer::Attack()
{
	if (!this->IsAlive())
	{
		return;
	}
	if (!this->bIsAttacking&&
		!(this->GetMovementComponent()->IsFalling()))
	{
		this->bIsAttacking = true;
		this->bInterpToEnemy = true;

		UAnimInstance* AnimInstance = this->GetMesh()->GetAnimInstance();
		if (AnimInstance&&this->AttackMontage)
		{
			const float PlayRate = FMath::RandRange(1.25f, 1.75f);
			const FString SectionName = FString::FromInt(FMath::RandRange(1, 2));
			AnimInstance->Montage_Play(this->AttackMontage,PlayRate);
			AnimInstance->Montage_JumpToSection(FName(*SectionName), this->AttackMontage);
		}
	}
}

void AMainPlayer::AttackEnd()
{
	this->bIsAttacking = false;
	this->bInterpToEnemy = false;
	if (!this->IsAlive())
	{
		return;
	}
	if (this->bAttackKeyDown)
	{
		AttackKeyDown();
	}
}

void AMainPlayer::UpdateAttackTarget()
{
	TArray<AActor*> OverlappingActors;
	//获取重叠的actor
	this->GetOverlappingActors(OverlappingActors, this->EnemyFilter);

	if (OverlappingActors.Num()==0)
	{
		this->AttackTarget = nullptr;
		return;
	}

	ABaseEnemy* ClosestEnemy = nullptr;
	float MinDistance = 1000.0f;
	const FVector Location = this->GetActorLocation();

	for (auto Actor: OverlappingActors)
	{
		ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor);
		if (Enemy&&Enemy->EnemyMovementStatus!=EEnemyMovementStatus::EEMS_Dead)
		{
			const float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
			if (DistanceToActor<MinDistance)
			{
				MinDistance = DistanceToActor;
				ClosestEnemy = Enemy;
			}
		}
	}
	this->AttackTarget = ClosestEnemy;
}

void AMainPlayer::Die()
{
	this->SetMovementStatus(EPlayerMovementStatus::EPMS_Dead);

	if (this->EquippedWeapon)
	{
		this->EquippedWeapon->DeactiveAttackCollision();
		this->EquippedWeapon->DeactiveDisplayMeshCollision();
	}

}

void AMainPlayer::DeathEnd()
{
	//暂停动画
	this->GetMesh()->bPauseAnims = true;
	//停止骨架更新
	this->GetMesh()->bNoSkeletonUpdate = true;

	FTimerHandle DeathTimerHandle;
	auto Lambda = [this]() {
		//TODO 
		RestartLevel();
	};

	GetWorldTimerManager().SetTimer(DeathTimerHandle, FTimerDelegate::CreateLambda(Lambda), 1.0f, false);
}

void AMainPlayer::RestartLevel()
{
	FString LevelName = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
	
}
