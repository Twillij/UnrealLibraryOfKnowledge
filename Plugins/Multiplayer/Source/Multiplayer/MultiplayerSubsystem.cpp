#include "MultiplayerSubsystem.h"
#include "MultiplayerFunctionLibrary.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSubsystem::UMultiplayerSubsystem()
{
	// Set the session delegates
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted);
	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleted);
	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleted);
	
	// Set the default session settings
	DefaultSessionSettings.NumPublicConnections = 4;
	DefaultSessionSettings.bIsDedicated = false;
	DefaultSessionSettings.bAllowInvites = true;
	DefaultSessionSettings.bAllowJoinInProgress = true;
	DefaultSessionSettings.bAllowJoinViaPresence = true;
	DefaultSessionSettings.bShouldAdvertise = true;
	DefaultSessionSettings.bUseLobbiesIfAvailable = true;
	DefaultSessionSettings.bUsesPresence = true;

	// Set the default session search settings
	DefaultSessionSearch.MaxSearchResults = 64;
	DefaultSessionSearch.QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
}

void UMultiplayerSubsystem::CreateSession(const FName SessionName)
{
	if (!SessionInterface)
		return;
	
	// Get the player's unique net ID
	const FUniqueNetIdRepl PlayerNetId = UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(GetWorld());
	
	// TODO: Allow for custom session settings in the future.
	FMultiplayerSessionInfo CustomSessionSettings;
	CustomSessionSettings.SessionName = SessionName;

	// Bind the delegate to the session interface and store the handle
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Create a new session settings by combining the default and custom settings.
	const FOnlineSessionSettings NewSessionSettings = CustomSessionSettings + DefaultSessionSettings;
	
	// Try to create a session using the specified params
	if (!SessionInterface->CreateSession(*PlayerNetId, SessionName, NewSessionSettings))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}
}

void UMultiplayerSubsystem::DestroySession(const FName SessionName)
{
	if (!SessionInterface)
		return;
	
	// Bind the delegate to the session interface and store the handle
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// Try to destroy a session using the specified params
	if (!SessionInterface->DestroySession(SessionName))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
}

void UMultiplayerSubsystem::FindSessions()
{
	if (!SessionInterface)
		return;
	
	// Get the player's unique net ID
	const FUniqueNetIdRepl PlayerNetId = UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(GetWorld());
	
	// Create and store a new session search
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch(DefaultSessionSearch));

	// Bind the delegate to the session interface and store the handle
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// Try to find sessions using the specified params
	if (!SessionInterface->FindSessions(*PlayerNetId, LastSessionSearch.ToSharedRef()))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
}

void UMultiplayerSubsystem::JoinSession(const FName SessionName, const FMultiplayerSessionSearchResult& SessionSearchResult)
{
	if (!SessionInterface)
		return;
	
	// Get the player's unique net ID
	const FUniqueNetIdRepl PlayerNetId = UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(GetWorld());

	// Try to join a session using the specified params
	if (!SessionInterface->JoinSession(*PlayerNetId, SessionName, SessionSearchResult.Result))
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
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
	
	if (!SessionInterface)
		return;
	
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
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("CREATE: %s"), *SessionName.ToString()));
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

	TArray<FMultiplayerSessionSearchResult> SessionSearchResults;
	TArray<FOnlineSessionSearchResult> Results = LastSessionSearch->SearchResults;
	UE_LOG(LogMultiplayer, Display, TEXT("Finding session: %i result"), Results.Num());
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("Session results: %i"), Results.Num()));
	
	for (int i = 0; i < Results.Num(); ++i)
	{
		SessionSearchResults.Add(Results[i]);

		FString SessionNameString;
		Results[i].Session.SessionSettings.Get(FName("SESSION_NAME"), SessionNameString);
		UE_LOG(LogMultiplayer, Display, TEXT("Session %i: %s"), i + 1, *SessionNameString)
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("Session: %s"), *SessionNameString));
	}
	
	OnSessionsFoundDelegate.Broadcast(SessionSearchResults);
}

void UMultiplayerSubsystem::OnJoinSessionCompleted(const FName SessionName, const EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface)
	{
		// Clear the bound delegate via the handle
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString ConnectInfo;
		SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo);

		OnSessionJoinedDelegate.Broadcast(SessionName, ConnectInfo);
		
		UE_LOG(LogMultiplayer, Display, TEXT("Connected to session \"%s\" at %s"), *SessionName.ToString(), *ConnectInfo);
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White,
			FString::Printf(TEXT("Connected to session \"%s\" at %s"), *SessionName.ToString(), *ConnectInfo));
	}
}
