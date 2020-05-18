// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerableDoor.h"
#include "Characters/Player/MainPlayer.h"
#include "TimerManager.h"

// Sets default values
ATriggerableDoor::ATriggerableDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	this->TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	this->TriggerBox->SetBoxExtent(FVector(60.0f, 60.0f, 30.0f));
	//设置组件本身的碰撞检测方式
	this->TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//设置组件本身的物体类型
	this->TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	//只开启Pawn通道的碰撞检测
	this->TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	this->TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	this->TriggerMesh->SetupAttachment(GetRootComponent());
	
	this->DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	this->DoorMesh->SetupAttachment(GetRootComponent());

	this->DelayTime = 1.5f;
	this->bIsPlayerOnTrigger = false;

}

// Called when the game starts or when spawned
void ATriggerableDoor::BeginPlay()
{
	Super::BeginPlay();
	
	this->TriggerBox->OnComponentBeginOverlap.AddDynamic(this,&ATriggerableDoor::OnOverlapBegin);
	this->TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ATriggerableDoor::OnOverlapEnd);
	this->InitTriggerLocation = this->TriggerMesh->GetComponentLocation();
	this->InitDoorLocation = this->DoorMesh->GetComponentLocation();
}

// Called every frame
void ATriggerableDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerableDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const AMainPlayer* MainPlayer=Cast<AMainPlayer>(OtherActor);
	if (MainPlayer)
	{
		GetWorldTimerManager().ClearTimer(this->CloseDoorTimerHandle);
		OpenDoor();
		LowerTrigger();
	}
}

void ATriggerableDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const AMainPlayer* MainPlayer = Cast<AMainPlayer>(OtherActor);
	if (MainPlayer)
	{
		RaiseTrigger();

		//auto DelayCloseDoor = [this]() {
		//	if (!this->bIsPlayerOnTrigger)
		//	{
		//		CloseDoor();
		//	}
		//};
		//Lambda表达式
		//GetWorldTimerManager().SetTimer(this->CloseDoorTimerHandle, FTimerDelegate::CreateLambda(DelayCloseDoor), this->DelayTime, false);

		//正常函数版本
		GetWorldTimerManager().SetTimer(this->CloseDoorTimerHandle,this,&ATriggerableDoor::CloseDoor,this->DelayTime);
		//CloseDoor();
	}
}

void ATriggerableDoor::UpdateTriggerLocation(FVector Offset)
{
	const FVector NewLocation = InitTriggerLocation + Offset;
	this->TriggerMesh->SetWorldLocation(NewLocation);
}

void ATriggerableDoor::UpdateDoorLocation(FVector Offset)
{
	const FVector NewLocation = InitDoorLocation + Offset;
	this->DoorMesh->SetWorldLocation(NewLocation);
}

