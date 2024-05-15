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

void AInkGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AInkGameState, mPlayersReady);
}

void AInkGameState::StartGame()
{
	if (mPlayersReady > 1)
	{
		GetWorld()->GetTimerManager().SetTimer(mGameTimerHandle, this, &AInkGameState::EndGame, 180.0f, false);
	}
}

void AInkGameState::EndGame()
{
}
