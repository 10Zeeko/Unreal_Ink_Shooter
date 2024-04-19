// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelComponents.h"
#include "PlayerHud.h"
#include "GameFramework/Actor.h"
#include "InkMeter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateInkMeter, int, Team1, int, Team2);

UCLASS()
class UNREAL_INK_SHOOTER_API AInkMeter : public AActor
{
	GENERATED_BODY()
	
public:	
	AInkMeter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ALevelComponents*> mpLevelComponents;

	UPROPERTY()
	FTimerHandle MCheckTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerHud*> mpPlayerHuds;

	UFUNCTION(Server, Unreliable)
	void RPC_Server_CheckInkFromLevelComponents();
	UFUNCTION(NetMulticast, Unreliable)
	void RPC_CheckInkFromLevelComponents(float aRedTeamInk, float aBlueTeamInk);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateInkMeter evOnUpdateInkMeter;

protected:
	UFUNCTION(Server, Reliable)
	void RPC_Server_FindAllLevelComponents();
	virtual void BeginPlay() override;
};
