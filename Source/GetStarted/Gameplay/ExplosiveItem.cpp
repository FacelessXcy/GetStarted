// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveItem.h"
#include "Characters/Player/MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "InteractableItem.h"
#include "../Characters/Enemy/BaseEnemy.h"

AExplosiveItem::AExplosiveItem()
{
	this->TriggerVolume->SetSphereRadius(50.0f);
	this->Damage = 20.0f;
}

void AExplosiveItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		ABaseEnemy* BaseEnemy = Cast<ABaseEnemy>(OtherActor);
		if (MainPlayer|| BaseEnemy)
		{
			//视觉与声音展示
			if (this->OverlapParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, this->OverlapParticle, GetActorLocation(), FRotator::ZeroRotator, true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			//发出伤害
			UGameplayStatics::ApplyDamage(OtherActor, this->Damage, nullptr, this, this->DamageTypeClass);
			Destroy();
		}
	}

}

void AExplosiveItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);


}
