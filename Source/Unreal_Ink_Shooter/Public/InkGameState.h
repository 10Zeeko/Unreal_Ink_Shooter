// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "InkGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdatePlayersReady, int, Team1);

UCLASS()
class UNREAL_INK_SHOOTER_API AInkGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AInkGameState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int mPlayersReady {0};
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdatePlayersReady evOnUpdatePlayersReady;

	FTimerHandle mGameTimerHandle;

	FTimerHandle mCheckPlayersReadyTimerHandle;

	void BeginPlay() override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RPC_Server_AddPlayerReady();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RPC_Server_CheckPlayersReady();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void EndGame();
};
