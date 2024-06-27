#include "MultiplayerGlobals.h"

DEFINE_LOG_CATEGORY(LogMultiplayer);

FOnlineSessionSettings FCustomSessionSettings::ToOnlineSessionSettings(const FOnlineSessionSettings& OnlineSessionSettings) const
{
	FOnlineSessionSettings Result = OnlineSessionSettings;
	Result.Set(FName("SESSION_NAME"), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Result.NumPublicConnections = NumPublicConnections;
	return Result;
}
