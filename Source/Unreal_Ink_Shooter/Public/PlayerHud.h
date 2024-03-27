// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHud.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateInkMeter, int, Team1, int, Team2);

UCLASS()
class UNREAL_INK_SHOOTER_API APlayerHud : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FUpdateInkMeter evOnUpdateInkMeter;
};
