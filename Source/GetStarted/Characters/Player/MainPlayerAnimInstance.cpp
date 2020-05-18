// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerAnimInstance.h"
#include "Characters/Player/MainPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainPlayerAnimInstance::NativeInitializeAnimation()
{
	PlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
}

void UMainPlayerAnimInstance::UpdateAnimationProperties()
{
	if (this->PlayerRef == nullptr)
	{
		this->PlayerRef = Cast<AMainPlayer>(TryGetPawnOwner());
	}

	if (this->PlayerRef)
	{
		const FVector SpeedVector = this->PlayerRef->GetVelocity();
		const FVector PlanarSpeed = FVector(SpeedVector.X, SpeedVector.Y, 0.0f);
		this->bIsInAir = this->PlayerRef->GetMovementComponent()->IsFalling();

		Speed = PlanarSpeed.Size();

		bIsInAir = PlayerRef->GetMovementComponent()->IsFalling();
	}
	
}
