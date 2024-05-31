// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/InkPlayerCharacter.h"
#include "InkGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdatePlayersReady, int, Team1);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndGame);

UCLASS()
class UNREAL_INK_SHOOTER_API AInkGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AInkGameState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int mPlayersReady {0};
	UPROPERTY()
	TArray<AInkPlayerCharacter*> mTeam1Players;
	UPROPERTY()
	TArray<AInkPlayerCharacter*> mTeam2Players;
	
	UPROPERTY()
	TArray<FVector> mTeam1SpawnPoints;
	
	UPROPERTY()
	TArray<FVector> mTeam2SpawnPoints;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdatePlayersReady evOnUpdatePlayersReady;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEndGame evOnEndGame;
	
	FTimerHandle mGameTimerHandle;

	FTimerHandle mCheckPlayersReadyTimerHandle;

	FTimerHandle mEndServerTimerHandle;

	void BeginPlay() override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RPC_Server_AddPlayerReady(AInkPlayerCharacter* aPlayer, bool team1);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RPC_Server_RemovePlayerReady(AInkPlayerCharacter* aPlayer, bool team1);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RPC_Server_CheckPlayersReady();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void EndGame();
};
