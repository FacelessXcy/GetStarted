// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	this->RootComponent = this->PlatformMesh;
	
	this->StartPoint = FVector::ZeroVector;
	this->EndPoint = FVector::ZeroVector;
	
	this->InterpSpeed = 200.0f;
	this->DelayTime = 2.0f;
	this->bInterping = true;

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	this->StartPoint = this->GetActorLocation();
	this->EndPoint += this->StartPoint;//转换为世界坐标
	this->Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->bInterping)
	{
		FVector CurrentLocation = this->GetActorLocation();
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, this->EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(NewLocation);
		
		float NowDistance = (GetActorLocation() - StartPoint).Size();
		if (Distance - NowDistance <= 0.5f)
		{
			bInterping = false;

			auto ToggleInterpState = [this]() 
			{
				this->bInterping = true;
			};
			GetWorldTimerManager().SetTimer(this->InterpTimerHandle, FTimerDelegate::CreateLambda(ToggleInterpState), this->DelayTime, false);

			FVector TempVec = this->StartPoint;
			this->StartPoint = this->EndPoint;
			this->EndPoint = TempVec;
		}
	}

	
}

