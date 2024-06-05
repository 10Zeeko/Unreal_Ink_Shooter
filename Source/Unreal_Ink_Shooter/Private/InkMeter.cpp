#include "InkMeter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

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
	evOnUpdateInkMeter.Broadcast(RedTeamInk, BlueTeamInk);
}

void AInkMeter::RPC_CheckInkFromLevelComponents_Implementation(float aRedTeamInk, float aBlueTeamInk)
{
	evOnUpdateInkMeter.Broadcast(aRedTeamInk, aBlueTeamInk);
}

void AInkMeter::RPC_Server_CheckFinalInkFromLevelComponents_Implementation()
{
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

	if (RedTeamInk > BlueTeamInk)
	{
		ScreenD("Red team won");
	}
	else if (BlueTeamInk > RedTeamInk)
	{
		ScreenD("Blue team won");
	}
	else
	{
		ScreenD("Draw");
	}
}

void AInkMeter::BeginPlay()
{
	Super::BeginPlay();
	if (mpLevelComponents.Num() == 0)
	{
		RPC_Server_FindAllLevelComponents();
	}
	GetWorld()->GetTimerManager().SetTimer(MCheckTimerHandle, this, &AInkMeter::RPC_Server_CheckInkFromLevelComponents, 5, true);
}

void AInkMeter::RPC_Server_FindAllLevelComponents_Implementation()
{
	TArray<AActor*> mLevelComponents;
	UGameplayStatics::GetAllActorsOfClass(this, ALevelComponents::StaticClass(), mLevelComponents);

	for (AActor* levelComponent : mLevelComponents)
	{
		mpLevelComponents.Add(Cast<ALevelComponents>(levelComponent));
	}
}
