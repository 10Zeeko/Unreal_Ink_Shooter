// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "InkPlayerState.generated.h"

UENUM(BlueprintType)
enum class ETeam
{
	NONE = 0 UMETA(DisplayName = "None"),
	TEAM1 UMETA(DisplayName = "Team 1"),
	TEAM2 UMETA(DisplayName = "Team 2"),
	TEAM3 UMETA(DisplayName = "Team 3")
};

UCLASS()
class UNREAL_INK_SHOOTER_API AInkPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	float mHP;
	UPROPERTY(BlueprintReadWrite)
	float mMaxHp = 100.0f;
	UPROPERTY(VisibleAnywhere)
	float mPlayerInk;
	UPROPERTY(BlueprintReadWrite)
	float mMaxInk = 100.0f;

	UPROPERTY()
	int mKills;
	UPROPERTY()
	int mDeaths;

	UPROPERTY(VisibleAnywhere)
	ETeam mPlayerTeam;

	void PlayerDamaged(float aDamage);

protected:
	void BeginPlay() override;
};
