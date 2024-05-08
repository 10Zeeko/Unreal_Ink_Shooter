#include "InkGameState.h"

#include "Kismet/GameplayStatics.h"

AInkGameState::AInkGameState()
{
}

void AInkGameState::StartGame()
{
	if (UGameplayStatics::GetNumPlayerStates(GetWorld()) > 1)
	{
		
	}
}

void AInkGameState::EndGame()
{
}
