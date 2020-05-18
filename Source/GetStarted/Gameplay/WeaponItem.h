// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/InteractableItem.h"
#include "Sound/SoundCue.h"
#include "WeaponItem.generated.h"

//��ʾ���������״̬
UENUM(BlueprintType)
enum class EWeaponState :uint8
{	
	EWS_CanPickup	UMETA(DisplayName="CanPickup"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
};
/**
 * 
 */
UCLASS()
class GETSTARTED_API AWeaponItem : public AInteractableItem
{
	GENERATED_BODY()
	
public:
	AWeaponItem();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weapon|Sound")
	USoundCue* OnEquippedSound;

	//�Ƿ�����Ч
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Particle")
	bool bShouldReserveIdleParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponState WeaponState;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class UBoxComponent* AttackCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack")
	class AController* WeaponOwner;

protected:
	virtual void BeginPlay() override;



public:

	//��ʰ�������
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	void Equip(class AMainPlayer* MainPlayer);
	void UnEquip(AMainPlayer* MainPlayer);

	void ActiveDisplayMeshCollision();
	void DeactiveDisplayMeshCollision();


	//�����������
	UFUNCTION()
	virtual void OnAttackCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAttackCollisionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActiveAttackCollision();

	UFUNCTION(BlueprintCallable)
	void DeactiveAttackCollision();
};
