#include "InkGameState.h"

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
	ScreenD(Format1("RPC_Server_AddPlayerReady_Implementation: %d", mPlayersReady));
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
	ScreenD(Format1("RPC_Server_RemovePlayerReady_Implementation: %d", mPlayersReady));
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
	GetWorld()->GetTimerManager().SetTimer(mGameTimerHandle, this, &AInkGameState::EndGame, 180.0f, false);
}

void AInkGameState::EndGame()
{
}
