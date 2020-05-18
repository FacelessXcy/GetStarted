// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GETSTARTED_API UBaseEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		float Speed;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
		class ABaseEnemy* EnemyRef;

public:


	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties();
};
