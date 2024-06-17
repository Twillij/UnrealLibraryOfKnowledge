#include "MultiplayerFunctionLibrary.h"
#include "Multiplayer.h"

FUniqueNetIdRepl UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(const UWorld* World)
{
	FUniqueNetIdRepl PlayerNetId;
	if (World)
	{
		const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
		PlayerNetId = LocalPlayer ? *LocalPlayer->GetCachedUniqueNetId() : PlayerNetId;
	}
	return PlayerNetId;
}

void UMultiplayerFunctionLibrary::LogNetMode(const UWorld* World)
{
	if (World)
	{
		UE_LOG(LogMultiplayer, Warning, TEXT("NetMode: %s"), *ToString(World->GetNetMode()));
	}
}
