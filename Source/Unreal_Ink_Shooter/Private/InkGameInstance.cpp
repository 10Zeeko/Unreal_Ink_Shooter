#include "InkGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Unreal_Ink_Shooter/Public/Utils.h"

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
	if (aJoinResult == EOnJoinSessionCompleteResult::Success)
	{
		LogD("[Server] - Joining server");

		if (APlayerController* PlayerController {UGameplayStatics::GetPlayerController(GetWorld(), 0)})
		{
			FString JoinAddress;
			mSessionInterface->GetResolvedConnectString(FName(aServerName), JoinAddress);

			if (!JoinAddress.IsEmpty())
			{
				PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else
	{
		LogD("[Server] - Joining server failed");
	}
}

void UInkGameInstance::OnFindServerCallback(bool aSucceded)
{
	if (aSucceded)
	{
		LogD("[Server] - Server found");

		FPackedServerData ServerData {};

		for (auto& ServerResult : mSessionsSearch->SearchResults)
		{
			FServerData Server {};
			Server.Name = ServerResult.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString();
			Server.Ping = ServerResult.PingInMs;
			Server.CurrentPlayers = (ServerResult.Session.SessionSettings.NumPublicConnections + ServerResult.Session.SessionSettings.NumPrivateConnections) - (ServerResult.Session.NumOpenPublicConnections + ServerResult.Session.NumOpenPrivateConnections) + 1;
			Server.MaxPlayers = ServerResult.Session.SessionSettings.NumPublicConnections + ServerResult.Session.SessionSettings.NumPrivateConnections;
			ServerData.Add(Server);
		}

		evOnFindSessions.Broadcast(ServerData);
	}
	else
	{
		LogD("[Server] - Server was not found");
	}
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

	FOnlineSessionSetting CompoundSessionName;
	CompoundSessionName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineServiceAndPing;
	CompoundSessionName.Data = aServer;
	Settings.Settings.Add(FName("SESSION_NAME"), CompoundSessionName);

	mSessionInterface->CreateSession(0, FName(aServer), Settings);
}

void UInkGameInstance::FindServers()
{
	LogD("[Server] - FindServers");

	mSessionsSearch = MakeShareable(new FOnlineSessionSearch());

	mSessionsSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	mSessionsSearch->MaxSearchResults = 5;
	mSessionsSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
	mSessionInterface->FindSessions(0, mSessionsSearch.ToSharedRef());
}

void UInkGameInstance::JoinServer(int aServerIndex)
{
	if (auto SearchResults {mSessionsSearch->SearchResults}; SearchResults.Num())
	{
		auto& ResultServer {SearchResults[aServerIndex]};
		auto ServerName {ResultServer.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString()};
		mCurrentSessionsName = ServerName;
		
		LogD2("[Server] - Joining server %d", aServerIndex);
		mSessionInterface->JoinSession(0, FName(ServerName), ResultServer);
	}
	else
	{
		LogD2("[Server] - Failed to join a server %d", aServerIndex);
	}
}
