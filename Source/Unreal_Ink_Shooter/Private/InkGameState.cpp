#include "InkGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AInkGameState::AInkGameState()
{
}

void AInkGameState::RPC_Server_AddPlayerReady_Implementation()
{
	++mPlayersReady;
}

void AInkGameState::BeginPlay()
{
	Super::BeginPlay();
	mPlayersReady = 0;

	GetWorld()->GetTimerManager().SetTimer(mCheckPlayersReadyTimerHandle, this, &AInkGameState::RPC_Server_CheckPlayersReady_Implementation, 5.0f, true);
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
