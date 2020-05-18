// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableItem.h"

// Sets default values
AInteractableItem::AInteractableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	this->RootComponent = this->TriggerVolume;

	this->TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	this->TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->TriggerVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	this->DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	this->DisplayMesh->SetupAttachment(this->GetRootComponent());

	this->IdleParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticleComponent"));
	this->IdleParticleComponent->SetupAttachment(this->GetRootComponent());

	this->bNeedRotate = true;
	this->RotationRate = 45.0f;
}

// Called when the game starts or when spawned
void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
	this->TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractableItem::OnOverlapBegin);
	this->TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractableItem::OnOverlapEnd);
}

// Called every frame
void AInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (this->bNeedRotate)
	{
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += this->RotationRate * DeltaTime;
		this->SetActorRotation(NewRotation);
	}
}

void AInteractableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AInteractableItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

