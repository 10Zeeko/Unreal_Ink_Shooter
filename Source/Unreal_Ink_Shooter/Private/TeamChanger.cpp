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
		inkGameState->RPC_Server_AddPlayerReady(mPlayerCharacter, selectedTeam ? true : false);
	}
}

void ATeamChanger::Server_OnOverlapEnd_Implementation(AActor* OtherActor)
{
	AInkPlayerCharacter* mPlayerCharacter = Cast<AInkPlayerCharacter>(OtherActor);
	if ( mPlayerCharacter && mPlayerCharacter->selectedWeapon != nullptr)
	{
		AInkGameState* inkGameState = Cast<AInkGameState>(GetWorld()->GetGameState());
		inkGameState->RPC_Server_RemovePlayerReady(mPlayerCharacter, selectedTeam ? true : false);
	}
}

void ATeamChanger::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority()) return;
}

