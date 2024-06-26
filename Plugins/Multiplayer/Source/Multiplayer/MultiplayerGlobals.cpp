#include "MultiplayerGlobals.h"

DEFINE_LOG_CATEGORY(LogMultiplayer);

FCustomSessionSettings::FCustomSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings)
{
	NumPublicConnections = OnlineSessionSettings.NumPublicConnections;
}

FOnlineSessionSettings FCustomSessionSettings::ToOnlineSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings) const
{
	FOnlineSessionSettings Result = OnlineSessionSettings;
	Result.NumPublicConnections = NumPublicConnections;
	return Result;
}
