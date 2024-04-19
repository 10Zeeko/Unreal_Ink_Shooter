#include "InkMeter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AInkMeter::AInkMeter()
{
}

void AInkMeter::RPC_Server_CheckInkFromLevelComponents_Implementation()
{
	// Get the ink values for the two teams
	TArray<FColor> TeamsColors = { FColor(255,0,0,0), FColor(0,0,255,0) };
	int RedTeamInk = 0;
	int BlueTeamInk = 0;

	for (ALevelComponents* apLevelComponent : mpLevelComponents)
	{
		if (ALevelComponents* levelC = Cast<ALevelComponents>(apLevelComponent))
		{
			levelC->RPC_Server_CheckInk(TeamsColors);
			RedTeamInk += levelC->mInkValues[0];
			BlueTeamInk += levelC->mInkValues[1];
		}
	}

	RPC_CheckInkFromLevelComponents(RedTeamInk, BlueTeamInk);
}

void AInkMeter::RPC_CheckInkFromLevelComponents_Implementation(float aRedTeamInk, float aBlueTeamInk)
{
	for (APlayerHud* playerHud : mpPlayerHuds)
	{
		if (playerHud)
		{
			playerHud->evOnUpdateInkMeter.Broadcast(aRedTeamInk, aBlueTeamInk);
		}
	}
}

void AInkMeter::BeginPlay()
{
	Super::BeginPlay();
	
	RPC_Server_FindAllLevelComponents();
}

void AInkMeter::RPC_Server_FindAllLevelComponents_Implementation()
{
	TArray<AActor*> mLevelComponents;
	UGameplayStatics::GetAllActorsOfClass(this, ALevelComponents::StaticClass(), mLevelComponents);

	for (AActor* levelComponent : mLevelComponents)
	{
		mpLevelComponents.Add(Cast<ALevelComponents>(levelComponent));
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController)
		{
			APlayerHud* PlayerHud = Cast<APlayerHud>(PlayerController->GetHUD());
			if (PlayerHud)
			{
				mpPlayerHuds.Add(PlayerHud);
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(MCheckTimerHandle, this, &AInkMeter::RPC_Server_CheckInkFromLevelComponents, 5, true);
}
