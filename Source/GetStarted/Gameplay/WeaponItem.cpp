// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Characters/Player/MainPlayer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "../Characters/Enemy/BaseEnemy.h"


AWeaponItem::AWeaponItem()
{
	this->TriggerVolume->SetSphereRadius(64.0f);

	//只是取消注册，标记要删除，并不是立即删除
	this->DisplayMesh->DestroyComponent();
	this->DisplayMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DisplaySkeletalMesh"));
	this->DisplayMesh->SetupAttachment(GetRootComponent());
	this->ActiveDisplayMeshCollision();

	this->AttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	this->AttackCollision->SetupAttachment(DisplayMesh, FName("WeaponSocket"));
	DeactiveAttackCollision();

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundCueAsset(TEXT("SoundCue'/Game/Assets/Audios/Blade_Cue.Blade_Cue'"));
	if (SoundCueAsset.Succeeded())
	{
		this->OnEquippedSound = SoundCueAsset.Object;
	}

	this->bShouldReserveIdleParticle = false;
	this->WeaponState = EWeaponState::EWS_CanPickup;
	this->Damage = 25.0f;
}

void AWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	this->AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponItem::OnAttackCollisionOverlapBegin);
	this->AttackCollision->OnComponentEndOverlap.AddDynamic(this, &AWeaponItem::OnAttackCollisionOverlapEnd);

}

void AWeaponItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor&&this->WeaponState==EWeaponState::EWS_CanPickup)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			MainPlayer->OverlappingWeapon = this;
		}
	}
}

void AWeaponItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer&& MainPlayer->OverlappingWeapon == this)
		{
			MainPlayer->OverlappingWeapon = nullptr;
		}
	}
}

void AWeaponItem::Equip(AMainPlayer* MainPlayer)
{
	if (MainPlayer)
	{
		this->WeaponState = EWeaponState::EWS_Equipped;
		this->DeactiveDisplayMeshCollision();

		//根据名字获取Socket
		const USkeletalMeshSocket* RightHandSocket = MainPlayer->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket)
		{
			//绑定武器到插槽上
			RightHandSocket->AttachActor(this, MainPlayer->GetMesh());
			MainPlayer->bHasWeapon = true;
			MainPlayer->EquippedWeapon = this;
			MainPlayer->OverlappingWeapon = nullptr;

			this->bNeedRotate = false;
			if (this->OnEquippedSound)
			{
				UGameplayStatics::PlaySound2D(this, this->OnEquippedSound);
			}
			if (!this->bShouldReserveIdleParticle)
			{
				this->IdleParticleComponent->Deactivate();
			}
		}
	}
}

void AWeaponItem::UnEquip(AMainPlayer* MainPlayer)
{
	if (MainPlayer&&
		!(MainPlayer->GetMovementComponent()->IsFalling())&&
		!(MainPlayer->bIsAttacking))
	{
		this->WeaponState = EWeaponState::EWS_CanPickup;
		this->ActiveDisplayMeshCollision();

		MainPlayer->bHasWeapon = false;
		MainPlayer->EquippedWeapon = nullptr;
		if (MainPlayer->OverlappingWeapon==nullptr)
		{
			MainPlayer->OverlappingWeapon = this;
		}
		this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		this->SetActorRotation(FRotator::ZeroRotator);
		this->SetActorScale3D(FVector(1.0f));

		this->bNeedRotate = true;
		this->IdleParticleComponent->Activate();
	}

}

void AWeaponItem::ActiveDisplayMeshCollision()
{
	this->DisplayMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//QueryOnly会影响Movement移动，PhysicalOnly似乎不会影响移动碰撞
	this->DisplayMesh->SetCollisionObjectType(ECC_WorldStatic);
	this->DisplayMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->DisplayMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

}

void AWeaponItem::DeactiveDisplayMeshCollision()
{
	this->DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponItem::OnAttackCollisionOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(OtherActor);
		if (BaseEnemy)
		{
			if (BaseEnemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = ((USkeletalMeshComponent*)this->DisplayMesh)->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					FVector SocketLocation = WeaponSocket->GetSocketLocation((USkeletalMeshComponent*)this->DisplayMesh);
					UGameplayStatics::SpawnEmitterAtLocation(this, BaseEnemy->HitParticles, SocketLocation, FRotator(0.0f), true);
				}
			}
			if (BaseEnemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, BaseEnemy->HitSound);
			}
			if (this->DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(BaseEnemy, this->Damage, this->WeaponOwner, this, this->DamageTypeClass);
			}
		}
	}
}

void AWeaponItem::OnAttackCollisionOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeaponItem::ActiveAttackCollision()
{
	this->AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//QueryOnly会影响Movement移动，PhysicalOnly似乎不会影响移动碰撞
	this->AttackCollision->SetCollisionObjectType(ECC_WorldDynamic);
	this->AttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWeaponItem::DeactiveAttackCollision()
{
	this->AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
