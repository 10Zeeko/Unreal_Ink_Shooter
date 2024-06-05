#include "InkGameState.h"

#include "InkMeter.h"
#include "ToolBuilderUtil.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

AInkGameState::AInkGameState()
{
}

void AInkGameState::BeginPlay()
{
	Super::BeginPlay();
	mPlayersReady = 0;
	mTeam1SpawnPoints.Add(FVector(-1700.0,-5590.0,540.0));
	mTeam1SpawnPoints.Add(FVector(-1700.0,-5890.0,540.0));
	mTeam1SpawnPoints.Add(FVector(-1850.0,-5440.0,540.0));
	mTeam1SpawnPoints.Add(FVector(-1550.0,-5440.0,540.0));

	mTeam2SpawnPoints.Add(FVector(1700.0,5590.0,540.0));
	mTeam2SpawnPoints.Add(FVector(1700.0,5890.0,540.0));
	mTeam2SpawnPoints.Add(FVector(1850.0,5440.0,540.0));
	mTeam2SpawnPoints.Add(FVector(1550.0,5440.0,540.0));

	GetWorld()->GetTimerManager().SetTimer(mCheckPlayersReadyTimerHandle, this, &AInkGameState::RPC_Server_CheckPlayersReady_Implementation, 5.0f, true);
}

void AInkGameState::RPC_Server_AddPlayerReady_Implementation(AInkPlayerCharacter* aPlayer, bool team1)
{
	++mPlayersReady;
	if (team1)
	{
		mTeam1Players.Add(aPlayer);
	}
	else
	{
		mTeam2Players.Add(aPlayer);
	}
}

void AInkGameState::RPC_Server_RemovePlayerReady_Implementation(AInkPlayerCharacter* aPlayer, bool team1)
{
	--mPlayersReady;
	if (team1)
	{
		mTeam1Players.Remove(aPlayer);
	}
	else
	{
		mTeam2Players.Remove(aPlayer);
	}
}

void AInkGameState::RPC_Server_CheckPlayersReady_Implementation()
{
	if (mPlayersReady > 1) StartGame();
}

void AInkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInkGameState, mPlayersReady);
}

void AInkGameState::StartGame()
{
 	GetWorld()->GetTimerManager().ClearTimer(mCheckPlayersReadyTimerHandle);
	mPlayersReady = 0;
	
	// Move players to spawn points
	for (int i = 0; i < mTeam1Players.Num(); ++i)
	{
		auto* Player {mTeam1Players[i]};
		Player->SetActorLocation(mTeam1SpawnPoints[i]);
		Player->mSpawnPoint = mTeam1SpawnPoints[i];
	}
	for (int i = 0; i < mTeam2Players.Num(); ++i)
	{
		auto* Player {mTeam2Players[i]};
		Player->SetActorLocation(mTeam2SpawnPoints[i]);
		Player->mSpawnPoint = mTeam2SpawnPoints[i];
	}
	GetWorld()->GetTimerManager().SetTimer(mGameTimerHandle, this, &AInkGameState::EndGame, 180.0f, true);

	ScreenD("Game started");
}

void AInkGameState::EndGame()
{
	ScreenD("Game ended");
	GetWorld()->GetTimerManager().ClearTimer(mGameTimerHandle);
	TArray<AInkPlayerState*> PlayerStatesArray;
	for (APlayerState* PlayerState : GetWorld()->GetGameState()->PlayerArray)
	{
		if (AInkPlayerState* InkPlayerState = Cast<AInkPlayerState>(PlayerState))
		{
			PlayerStatesArray.Add(InkPlayerState);
		}
	}

	AInkMeter *InkMeter = Cast<AInkMeter>(UGameplayStatics::GetActorOfClass(GetWorld(), AInkMeter::StaticClass()));
	InkMeter->RPC_Server_CheckFinalInkFromLevelComponents();

	evOnEndGame.Broadcast();
}
