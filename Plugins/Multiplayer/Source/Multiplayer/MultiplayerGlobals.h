#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerGlobals.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMultiplayer, Display, All)

USTRUCT(BlueprintType)
struct FMultiplayerSessionInfo
{
	GENERATED_BODY()

	FMultiplayerSessionInfo() {}
	FMultiplayerSessionInfo(const FOnlineSession& Session);
	FMultiplayerSessionInfo(const FOnlineSessionSearchResult& SessionSearchResult) { FMultiplayerSessionInfo(SessionSearchResult.Session); }
	
	// The name of the session
	UPROPERTY(BlueprintReadWrite)
	FName SessionName = NAME_None;

	// Combines the two settings into one
	FOnlineSessionSettings operator+(const FOnlineSessionSettings& OnlineSessionSettings) const;
};

// A wrapper for FOnlineSessionSearchResult that is intended for blueprint use
USTRUCT(BlueprintType)
struct FMultiplayerSessionSearchResult
{
	GENERATED_BODY()

	FMultiplayerSessionSearchResult() {}
	FMultiplayerSessionSearchResult(const FOnlineSessionSearchResult& SessionSearchResult) : Result(SessionSearchResult) {}
	
	FOnlineSessionSearchResult Result;
};
