#include "MultiplayerSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

DEFINE_LOG_CATEGORY(LogMultiplayer);

void UMultiplayerSubsystem::CreateSession(const FName& SessionName)
{
	FOnlineSessionSettings NewSessionSettings = DefaultSessionSettings;
	SessionInterface->CreateSession(0, SessionName, NewSessionSettings);
}

void UMultiplayerSubsystem::DestroySession(const FName& SessionName)
{
	SessionInterface->DestroySession(SessionName);
}

void UMultiplayerSubsystem::FindSession(const FName SessionName)
{
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch(DefaultSessionSearch));
	SessionInterface->FindSessions(0, LastSessionSearch.ToSharedRef());
}

void UMultiplayerSubsystem::JoinSession(const FName SessionName)
{
	// TODO: Implement join session.
}

void UMultiplayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		UE_LOG(LogMultiplayer, Error, TEXT("Failed to connect to a valid OSS."))
		return;
	}

	// Store the name of the subsystem
	OnlineSubsystemName = OnlineSubsystem->GetSubsystemName();
	UE_LOG(LogMultiplayer, Display, TEXT("Connected to OSS: %s"), *OnlineSubsystemName.ToString());

	// Bind delegates to the session interface
	SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface)
	{
		//SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnCreateSessionComplete);
		//SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnDestroySessionComplete);
		//SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSubsystem::OnFindSessionComplete);
	}

	// Create the default settings for online sessions
	DefaultSessionSettings.bIsLANMatch = OnlineSubsystemName == "NULL";
	DefaultSessionSettings.NumPublicConnections = 2;
	DefaultSessionSettings.bIsDedicated = true;
	DefaultSessionSettings.bAllowJoinInProgress = true;
	DefaultSessionSettings.bShouldAdvertise = true;
	DefaultSessionSettings.bUseLobbiesIfAvailable = true;
	DefaultSessionSettings.bUsesPresence = true;
	DefaultSessionSettings.bAllowJoinViaPresence = true;

	// Create the default settings for online searches
	DefaultSessionSearch.bIsLanQuery = OnlineSubsystemName == "NULL";
	DefaultSessionSearch.MaxSearchResults = 64;
	DefaultSessionSearch.QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
}
