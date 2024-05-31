#include "TeamChanger.h"

#include "InkGameState.h"
#include "Player/InkPlayerCharacter.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"
#include "Weapons/Weapon.h"

class AInkPlayerCharacter;

ATeamChanger::ATeamChanger()
{
	mpBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	RootComponent = mpBase;

	mpHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	mpHitbox->SetupAttachment(RootComponent);
	mpHitbox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void ATeamChanger::Server_OnOverlapBegin_Implementation(AActor* OtherActor)
{
	AInkPlayerCharacter* mPlayerCharacter = Cast<AInkPlayerCharacter>(OtherActor);
	if ( mPlayerCharacter && mPlayerCharacter->selectedWeapon != nullptr)
	{
		mPlayerCharacter->RPC_Server_UpdatePlayerTeam(selectedTeam);
		AInkGameState* inkGameState = Cast<AInkGameState>(GetWorld()->GetGameState());
		switch (selectedTeam)
		{
		case 0:
			inkGameState->RPC_Server_AddPlayerReady(mPlayerCharacter, true);
			break;
		case 1:
			inkGameState->RPC_Server_AddPlayerReady(mPlayerCharacter, false);
			break;
		default:
			inkGameState->RPC_Server_AddPlayerReady(mPlayerCharacter, true);
			break;
		}
	}
}

void ATeamChanger::Server_OnOverlapEnd_Implementation(AActor* OtherActor)
{
	AInkPlayerCharacter* mPlayerCharacter = Cast<AInkPlayerCharacter>(OtherActor);
	if ( mPlayerCharacter && mPlayerCharacter->selectedWeapon != nullptr)
	{
		AInkGameState* inkGameState = Cast<AInkGameState>(GetWorld()->GetGameState());
		switch (selectedTeam)
		{
		case 0:
			inkGameState->RPC_Server_RemovePlayerReady(mPlayerCharacter, true);
			break;
		case 1:
			inkGameState->RPC_Server_RemovePlayerReady(mPlayerCharacter, false);
			break;
		default:
			inkGameState->RPC_Server_RemovePlayerReady(mPlayerCharacter, true);
			break;
		}
	}
}

void ATeamChanger::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority()) return;
}

