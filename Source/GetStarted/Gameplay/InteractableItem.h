// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Components/MeshComponent.h"
#include "InteractableItem.generated.h"

UCLASS()
class GETSTARTED_API AInteractableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableItem();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	USphereComponent* TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMeshComponent* DisplayMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UParticleSystemComponent* IdleParticleComponent;//��Ϊ���

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Item|Particles")
	UParticleSystem* OverlapParticle;//��Ϊ��Դ

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Item|Sounds")
	USoundCue* OverlapSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Interactable Item|Item Properties")
	bool bNeedRotate;//�Ƿ���Ҫ��ת��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Item|Item Properties")
	float RotationRate;//��ת����
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
