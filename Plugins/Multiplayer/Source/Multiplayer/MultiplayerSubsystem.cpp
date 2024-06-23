#include "MultiplayerSubsystem.h"
#include "MultiplayerGlobals.h"
#include "MultiplayerFunctionLibrary.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSubsystem::UMultiplayerSubsystem()
{
	// Set the session delegates
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted);
	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleted);
	
	// Set the default session settings
	DefaultSessionSettings.NumPublicConnections = 2;
	DefaultSessionSettings.bIsDedicated = true;
	DefaultSessionSettings.bAllowJoinInProgress = true;
	DefaultSessionSettings.bShouldAdvertise = true;
	DefaultSessionSettings.bUseLobbiesIfAvailable = true;
	DefaultSessionSettings.bUsesPresence = true;
	DefaultSessionSettings.bAllowJoinViaPresence = true;

	// Set the default session search settings
	DefaultSessionSearch.MaxSearchResults = 64;
	DefaultSessionSearch.QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
}

void UMultiplayerSubsystem::CreateSession(const FName SessionName)
{
	// Get the player's unique net ID
	const FUniqueNetIdRepl PlayerNetId = UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(GetWorld());
	
	// TODO: Allow for custom session settings in the future.
	FCustomSessionSettings CustomSessionSettings;

	// Bind the delegate to the session interface and store the handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	
	// Try to create a session using the specified params
	if (!SessionInterface->CreateSession(*PlayerNetId, SessionName, CustomSessionSettings.ToOnlineSessionSettings(DefaultSessionSettings)))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
}

void UMultiplayerSubsystem::DestroySession(const FName SessionName)
{
	// TODO: Do all this logic on server only.
	
	// Bind the delegate to the session interface and store the handle
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
	
	if (!SessionInterface->DestroySession(SessionName))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
}

void UMultiplayerSubsystem::FindSessions()
{
	// Create and store a new session search
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch(DefaultSessionSearch));

	// Bind the delegate to the session interface and store the handle
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// Try to find sessions using the player's net ID and session search settings
	if (!SessionInterface->FindSessions(*UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(GetWorld()), LastSessionSearch.ToSharedRef()))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
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
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionCompleted);
	}

	// Determine whether this is a LAN match or not
	DefaultSessionSettings.bIsLANMatch = OnlineSubsystemName == "NULL";
	DefaultSessionSearch.bIsLanQuery = OnlineSubsystemName == "NULL";
}

void UMultiplayerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	if (SessionInterface)
	{
		const int HostedSessionCount = HostedSessionNames.Num();
		int Iterator = 0;
		
		// Get all the session names hosted by this machine.
		for (int i = 0; i < HostedSessionCount; ++i)
		{
			// If a session with that name exists on the server...
			if (const FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(HostedSessionNames[Iterator]))
			{
				// And there are no registered players in that session...
				if (NamedSession && NamedSession->RegisteredPlayers.Num() == 0)
				{
					// Destroy that session.
					if (!SessionInterface->DestroySession(HostedSessionNames[Iterator]))
					{
						// If we failed to destroy the session, move on the next one.
						Iterator++;
					}
				}
			}
		}
	}
}

void UMultiplayerSubsystem::OnCreateSessionCompleted(const FName SessionName, const bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
	if (bWasSuccessful)
	{
		HostedSessionNames.Add(SessionName);
	}
	UE_LOG(LogMultiplayer, Display, TEXT("Creating session \"%s\": %hs"), *SessionName.ToString(), bWasSuccessful ? "SUCCESS" : "FAILED")
}

void UMultiplayerSubsystem::OnDestroySessionCompleted(const FName SessionName, const bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	if (bWasSuccessful)
	{
		HostedSessionNames.Remove(SessionName);
	}
	UE_LOG(LogMultiplayer, Display, TEXT("Destroying session \"%s\": %hs"), *SessionName.ToString(), bWasSuccessful ? "SUCCESS" : "FAILED")
}

void UMultiplayerSubsystem::OnFindSessionsCompleted(const bool bWasSuccessful)
{
	if (SessionInterface)
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
	if (!bWasSuccessful || !LastSessionSearch)
	{
		UE_LOG(LogMultiplayer, Display, TEXT("Finding session: FAILED"))
		return;
	}
	UE_LOG(LogMultiplayer, Display, TEXT("Finding session: %i result"), LastSessionSearch->SearchResults.Num());
}
