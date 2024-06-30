#include "MultiplayerGlobals.h"

DEFINE_LOG_CATEGORY(LogMultiplayer);

FMultiplayerSessionInfo::FMultiplayerSessionInfo(const FOnlineSession& Session)
{
	FString NameString;
	Session.SessionSettings.Get(FName("SESSION_NAME"), NameString);
	SessionName = FName(NameString);
}

FOnlineSessionSettings FMultiplayerSessionInfo::operator+(const FOnlineSessionSettings& OnlineSessionSettings) const
{
	FOnlineSessionSettings Result = OnlineSessionSettings;
	Result.Set(FName("SESSION_NAME"), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	return Result;
}
