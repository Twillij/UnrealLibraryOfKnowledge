#pragma once

#include "CoreMinimal.h"
#include "MultiplayerGlobals.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsFoundDelegate, const TArray<FMultiplayerSessionSearchResult>&, SessionSearchResults);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionJoinedDelegate, const FName&, SessionName, const FString&, ConnectInfo);

UCLASS(Config=Game)
class MULTIPLAYER_API UMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSubsystem();
	
	FOnlineSessionSettings DefaultSessionSettings;
	FOnlineSessionSearch DefaultSessionSearch;

	UPROPERTY(BlueprintAssignable)
	FOnSessionsFoundDelegate OnSessionsFoundDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSessionJoinedDelegate OnSessionJoinedDelegate;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	FName OnlineSubsystemName = NAME_None;
	
	IOnlineSessionPtr SessionInterface = nullptr;
	TArray<FName> HostedSessionNames;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

public:
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName SessionName);

	UFUNCTION(BlueprintCallable)
	void DestroySession(const FName SessionName);

	UFUNCTION(BlueprintCallable)
	void FindSessions();

	UFUNCTION(BlueprintCallable)
	void JoinSession(const FName SessionName, const FMultiplayerSessionSearchResult& SessionSearchResult);
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnCreateSessionCompleted(const FName SessionName, const bool bWasSuccessful);
	void OnDestroySessionCompleted(const FName SessionName, const bool bWasSuccessful);
	void OnFindSessionsCompleted(const bool bWasSuccessful);
	void OnJoinSessionCompleted(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result);
};
