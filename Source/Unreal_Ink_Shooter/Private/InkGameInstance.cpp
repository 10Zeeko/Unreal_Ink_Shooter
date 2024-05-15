#define ScreenD(x) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}
#define ScreenDv(x) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, 2.0f, FColor::Yellow, x);}

#define ScreenDt(x, y) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, y.0f, FColor::Yellow, TEXT(x));}
#define ScreenDv(x, y) if (GEngine){GEngine->AddOnScrenDebugMessage(-1, y.0f, FColor::Yellow, x);}

#define LogD(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define LogD2(x, y) UE_LOG(LogTemp, Warning, TEXT(x), y);
#define LogDt(x) UE_LOG(LogTemp, Warning, x);

#define Format1(x, y) FString::Printf(TEXT(x), y);
#define Format1(x, y, z) FString::Printf(TEXT(x), y, z);
#define Format1(x, y, z, a) FString::Printf(TEXT(x), y, z, a);
#define Format1(x, y, z, a, b) FString::Printf(TEXT(x), y, z, a, b);
#define Format1(x, y, z, a, b, c) FString::Printf(TEXT(x), y, z, a, b, c);

#include "InkGameInstance.h"

UInkGameInstance::UInkGameInstance()
{
}

void UInkGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* Subsystem {IOnlineSubsystem::Get()})
	{
		mSessionInterface = Subsystem->GetSessionInterface();

		if (mSessionInterface.IsValid())
		{
			mSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UInkGameInstance::OnCreateServerCallback);
			mSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UInkGameInstance::OnJoinServerCallback);
			mSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UInkGameInstance::OnFindServerCallback);
		}
	}
}

void UInkGameInstance::OnCreateServerCallback(FName aServerName, bool aSucceded)
{
	if (aSucceded)
	{
		LogD("[Server] - Server created");

		if (UWorld* World {GetWorld()})
		{
			World->ServerTravel(mGameMap);
		}
	}
	else
	{
		LogD("[Server] - Server was not created");
	}
	
}

void UInkGameInstance::OnJoinServerCallback(FName aServerName, EOnJoinSessionCompleteResult::Type aJoinResult)
{
}

void UInkGameInstance::OnFindServerCallback(bool aSucceded)
{
}

void UInkGameInstance::CreateServer(FString aServer)
{
	LogD("[Server] - CreateServer");

	FOnlineSessionSettings Settings;
	Settings.bAllowJoinInProgress = true;
	Settings.bIsDedicated = false;
	Settings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	Settings.bShouldAdvertise = true;
	Settings.bUsesPresence = true;
	Settings.NumPublicConnections = 3;

	mSessionInterface->CreateSession(0, FName(aServer), Settings);
}

void UInkGameInstance::FindServers()
{
}

void UInkGameInstance::JoinServer(int aServerIndex)
{
}
