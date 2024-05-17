// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "InkGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()
	FServerData(){}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int Ping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int MaxPlayers;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int CurrentPlayers;
};

USTRUCT(BlueprintType)
struct FPackedServerData
{
	GENERATED_BODY()
	FPackedServerData(){}

	void Add(FServerData aOther) {ServerList.Add(aOther); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FServerData> ServerList {};
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindSessions, FPackedServerData, apSessions);

/**
 * 
 */
UCLASS()
class UInkGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UInkGameInstance();

protected:
	virtual void Init() override;
	
	virtual void OnCreateServerCallback(FName aServerName, bool aSucceded);
	virtual void OnJoinServerCallback(FName aServerName, EOnJoinSessionCompleteResult::Type aJoinResult);
	virtual void OnFindServerCallback(bool aSucceded);

	UFUNCTION(BlueprintCallable)
	virtual void CreateServer(FString aServer);
	UFUNCTION(BlueprintCallable)
	virtual void FindServers();
	UFUNCTION(BlueprintCallable)
	virtual void JoinServer(int aServerIndex);

	TSharedPtr<FOnlineSessionSearch> mSessionsSearch;
	IOnlineSessionPtr mSessionInterface;
	
	FString mGameMap {"/Game/Levels/TestLevel?listen"};

private:
	UPROPERTY(BlueprintAssignable)
	FOnFindSessions evOnFindSessions;

	UPROPERTY()
	FString mCurrentSessionsName;
};
