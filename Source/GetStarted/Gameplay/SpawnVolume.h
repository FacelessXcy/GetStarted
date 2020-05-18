// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "SpawnVolume.generated.h"

UCLASS()
class GETSTARTED_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* SpawnBox;

	//TSubclassOf<AActor>  AActor�����඼���Ա�����������ģ���� ��ʱ����Щ�����Ϊ�����е�GameObject
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Spawn Volume")
	TArray<TSubclassOf<AActor>> SpawnActorsClassesArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//��ȡ������ɵ�
	UFUNCTION(BlueprintPure, Category = "Spawn Volume")
	FVector GetSpawnPoint();

	//��ȡ������ɵ�����
	UFUNCTION(BlueprintPure, Category = "Spawn Volume")
	TSubclassOf<AActor> GetSpawnActorClass();

	//��������
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Spawn Volume")
	void SpawnActor(UClass* SpawnClass,FVector SpawnLocation);
};
