// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "InkGameState.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL_INK_SHOOTER_API AInkGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AInkGameState();


	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void EndGame();
};
