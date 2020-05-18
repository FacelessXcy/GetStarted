// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnimInstance.h"
#include "Characters/Enemy/BaseEnemy.h"


void UBaseEnemyAnimInstance::NativeInitializeAnimation()
{
	this->EnemyRef = Cast<ABaseEnemy>(TryGetPawnOwner());
}

void UBaseEnemyAnimInstance::UpdateAnimationProperties()
{
	if (this->EnemyRef == nullptr)
	{
		this->EnemyRef = Cast<ABaseEnemy>(TryGetPawnOwner());
	}

	if (this->EnemyRef)
	{
		const FVector SpeedVector = this->EnemyRef->GetVelocity();
		const FVector PlanarSpeed = FVector(SpeedVector.X, SpeedVector.Y, 0.0f);

		Speed = PlanarSpeed.Size();

		//float Direction this->CalculateDirection(EnemyRef->GetVelocity(), EnemyRef->GetActorRotation());
	}
}
