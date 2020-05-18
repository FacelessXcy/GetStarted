// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/InteractableItem.h"
#include "PickupItem.generated.h"

/**
 * 
 */
UCLASS()
class GETSTARTED_API APickupItem : public AInteractableItem
{
	GENERATED_BODY()
	
public:
	APickupItem();


public:

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION(BlueprintImplementableEvent,Category="Pickup")
	void OnPickUp(class AMainPlayer* MainPlayer);

};
