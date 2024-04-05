#include "InkMeter.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AInkMeter::AInkMeter()
{
}

void AInkMeter::CheckInkFromLevelComponents()
{
	// Get the ink values for the two teams
	TArray<FColor> TeamsColors = { FColor(255,0,0,0), FColor(0,0,255,0) };
	int RedTeamInk = 0;
	int BlueTeamInk = 0;

	for (ALevelComponents* apLevelComponent : apLevelComponents)
	{
		if (ALevelComponents* levelC = Cast<ALevelComponents>(apLevelComponent))
		{
			levelC->CheckInk(TeamsColors);
			RedTeamInk += levelC->inkValues[0];
			BlueTeamInk += levelC->inkValues[1];
		}
	}

	if (apPlayerHud)
	{
		apPlayerHud->evOnUpdateInkMeter.Broadcast(RedTeamInk, BlueTeamInk);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("apPlayerHud is null."));
	}
}

void AInkMeter::BeginPlay()
{
	Super::BeginPlay();

	if (apLevelComponents.Num() == 0)
	{
		FindAllLevelComponents();
	}
	apPlayerHud = Cast<APlayerHud>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD());
	GetWorld()->GetTimerManager().SetTimer(MCheckTimerHandle, this, &AInkMeter::CheckInkFromLevelComponents, 5, true);
}

void AInkMeter::FindAllLevelComponents()
{
	TArray<AActor*> mLevelComponents;
	UGameplayStatics::GetAllActorsOfClass(this, ALevelComponents::StaticClass(), mLevelComponents);

	for (AActor* levelComponent : mLevelComponents)
	{
		apLevelComponents.Add(Cast<ALevelComponents>(levelComponent));
	}
}
