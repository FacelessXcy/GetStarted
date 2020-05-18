// Fill out your copyright notice in the Description page of Project Settings.


#include "Level1GameMode.h"
#include "Kismet/GameplayStatics.h"

void ALevel1GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (this->MainUIClass)
	{
		this->MainUI = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(this, 0), this->MainUIClass);
		if (this->MainUI)
		{
			this->MainUI->AddToViewport();
		}
	}
}
