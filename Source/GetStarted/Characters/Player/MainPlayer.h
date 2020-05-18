// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "../../Gameplay/WeaponItem.h"

#include "MainPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlayerMovementStatus :uint8
{
	EPMS_Normal UMETA(DisplayName="Normal"),
	EPMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EPMS_Dead UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class EPlayerStaminaStatus :uint8
{
	EPSS_Normal UMETA(DisplayName = "Normal"),
	EPSS_Exhausted UMETA(DisplayName = "Exhausted"),
	EPSS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
};

UCLASS()
class GETSTARTED_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainPlayer();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UCameraComponent* FollowCamera;

	float BaseTurnRate;

	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float StaminaConsumeRate;//耐力消耗速率

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats",meta=(ClampMin=0.0f,ClampMax=1.0f))
	float ExhaustedStaminaRatio;//疲劳区比例

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Player Stats")
	EPlayerStaminaStatus StaminaStatus;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	float SprintingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats")
	EPlayerMovementStatus MovementStatus;

	bool bLeftShiftKeyDown;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Weapon")
	bool bHasWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponItem* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeaponItem* OverlappingWeapon;

	bool bAttackKeyDown;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Animation")
	bool bIsAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* AttackMontage;//动画蒙太奇资源

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	class ABaseEnemy* AttackTarget;

	//Overlap过滤器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<ABaseEnemy> EnemyFilter;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Attack")
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bInterpToEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	class USoundCue* HitSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;


	//Axis
	void MoveForward(float Value);
	void MoveRight(float Value);

	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	UFUNCTION(BlueprintCallable)
	void IncreaseHealth(float Value);
	UFUNCTION(BlueprintCallable)
	void IncreaseStamina(float Value);
	UFUNCTION(BlueprintCallable)
	void IncreaseCoin(int32 Value);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE void LeftShiftKeyDown()
	{
		this->bLeftShiftKeyDown = true;
	}
	FORCEINLINE void LeftShiftKeyUp()
	{
		this->bLeftShiftKeyDown = false;
	}

	void SetMovementStatus(EPlayerMovementStatus Status);

	void InteractKeyDown();

	void AttackKeyDown();

	FORCEINLINE void AttackKeyUp() {
		this->bAttackKeyDown = false;
	}

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();


	//更新攻击的目标
	void UpdateAttackTarget();

	void Die();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	FORCEINLINE bool IsAlive()
	{
		return this->MovementStatus != EPlayerMovementStatus::EPMS_Dead;
	}

	void RestartLevel();
};
