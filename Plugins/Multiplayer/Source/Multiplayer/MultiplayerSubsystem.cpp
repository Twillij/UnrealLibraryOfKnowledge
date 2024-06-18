#include "MultiplayerSubsystem.h"
#include "Multiplayer.h"
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
	// TODO: Do all this logic on server only.
	
	FOnlineSessionSettings NewSessionSettings = DefaultSessionSettings;

	// Bind the delegate to the session interface and store the handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Try to create a session using the player's net ID and session settings
	if (!SessionInterface->CreateSession(*UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(GetWorld()), SessionName, NewSessionSettings))
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

	// If this is not a client...
	if (SessionInterface && GetWorld() && GetWorld()->GetNetMode() < NM_Client)
	{
		// Get all the active session names.
		for (int i = 0; i < ActiveSessionNames.Num(); ++i)
		{
			// If a session with that name exists on the server...
			if (const FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(ActiveSessionNames[i]))
			{
				// And there are no registered players in that session...
				if (NamedSession->RegisteredPlayers.Num() == 0)
				{
					// Destroy that session.
					SessionInterface->DestroySession(ActiveSessionNames[i]);
				}
			}
		}
		ActiveSessionNames.Empty();
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
		ActiveSessionNames.Add(SessionName);
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
		ActiveSessionNames.Remove(SessionName);
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
