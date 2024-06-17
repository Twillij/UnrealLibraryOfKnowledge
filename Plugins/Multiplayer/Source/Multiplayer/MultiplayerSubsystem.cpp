#include "MultiplayerSubsystem.h"
#include "Multiplayer.h"
#include "MultiplayerFunctionLibrary.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSubsystem::UMultiplayerSubsystem()
{
	// Set the session delegates
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	DestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete);
	
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
	FOnlineSessionSettings NewSessionSettings = DefaultSessionSettings;

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	
	SessionInterface->CreateSession(0, SessionName, NewSessionSettings);
}

void UMultiplayerSubsystem::DestroySession(const FName SessionName)
{
	SessionInterface->DestroySession(SessionName);
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
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &ThisClass::OnDestroySessionComplete);
	}

	// Determine whether this is a LAN match or not
	DefaultSessionSettings.bIsLANMatch = OnlineSubsystemName == "NULL";
	DefaultSessionSearch.bIsLanQuery = OnlineSubsystemName == "NULL";
}

void UMultiplayerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (GetWorld() && GetWorld()->GetNetMode() < NM_Client)
	{
		for (const FName SessionName : CreatedSessionNames)
		{
			if (const FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(SessionName))
			{
				if (NamedSession->RegisteredPlayers.Num() == 0)
				{
					SessionInterface->DestroySession(SessionName);
				}
			}
		}
	}
}

void UMultiplayerSubsystem::OnCreateSessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		CreatedSessionNames.Add(SessionName);
	}
	UE_LOG(LogMultiplayer, Display, TEXT("Creating session \"%s\": %hs"), *SessionName.ToString(), bWasSuccessful ? "SUCCESS" : "FAILED")
}

void UMultiplayerSubsystem::OnDestroySessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		CreatedSessionNames.Remove(SessionName);
	}
	UE_LOG(LogMultiplayer, Display, TEXT("Destroying session \"%s\": %hs"), *SessionName.ToString(), bWasSuccessful ? "SUCCESS" : "FAILED")
}

void UMultiplayerSubsystem::OnFindSessionsComplete(const bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}
	
	if (!bWasSuccessful || !LastSessionSearch)
	{
		UE_LOG(LogMultiplayer, Display, TEXT("Finding session: FAILED"))
		return;
	}
	
	UE_LOG(LogMultiplayer, Display, TEXT("Finding session: %i result"), LastSessionSearch->SearchResults.Num());
}
