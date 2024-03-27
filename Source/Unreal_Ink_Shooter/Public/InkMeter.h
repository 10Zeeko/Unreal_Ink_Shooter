// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelComponents.h"
#include "PlayerHud.h"
#include "GameFramework/Actor.h"
#include "InkMeter.generated.h"

UCLASS()
class UNREAL_INK_SHOOTER_API AInkMeter : public AActor
{
	GENERATED_BODY()
	
public:	
	AInkMeter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ALevelComponents*> apLevelComponents;

	FTimerHandle MCheckTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerHud* apPlayerHud;

	void CheckInkFromLevelComponents();
protected:
	virtual void BeginPlay() override;
};
