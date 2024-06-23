#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerGlobals.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMultiplayer, Display, All)

// Online session settings that are customisable from the client side
USTRUCT(BlueprintType)
struct FCustomSessionSettings
{
	GENERATED_BODY()

	FCustomSessionSettings() {}
	FCustomSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings);

	// The number of publicly available connections advertised
	int32 NumPublicConnections = 2;

	FOnlineSessionSettings ToOnlineSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings) const;
};
