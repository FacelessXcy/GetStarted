// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItem.h"
#include "Characters/Player/MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

APickupItem::APickupItem()
{
	this->TriggerVolume->SetSphereRadius(50.0f);
}

void APickupItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
		if (MainPlayer)
		{
			if (OverlapParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(this, this->OverlapParticle, this->GetActorLocation(), this->GetActorRotation(), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, this->OverlapSound);
			}
			//
			OnPickUp(MainPlayer);
			Destroy();
		}
	}
}

void APickupItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);


}


