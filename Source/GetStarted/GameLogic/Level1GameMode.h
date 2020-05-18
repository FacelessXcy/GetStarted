// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GetStartedGameModeBase.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "Level1GameMode.generated.h"

/**
 * 
 */
UCLASS()
class GETSTARTED_API ALevel1GameMode : public AGetStartedGameModeBase
{
	GENERATED_BODY()

public:
	//��Ҫ���ɵ�UI��
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="UI Widgets")
	TSubclassOf<class UUserWidget> MainUIClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI Widgets")
	UUserWidget* MainUI;//����Ѿ����ɵ�UI��ʵ��


protected:
	virtual void BeginPlay() override;

};
