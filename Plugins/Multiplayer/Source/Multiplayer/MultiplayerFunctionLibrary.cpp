#include "MultiplayerFunctionLibrary.h"
#include "MultiplayerGlobals.h"

FUniqueNetIdRepl UMultiplayerFunctionLibrary::GetLocalPlayerUniqueNetId(const UWorld* World)
{
	FUniqueNetIdRepl PlayerNetId;
	if (World)
	{
		const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
		PlayerNetId = LocalPlayer ? LocalPlayer->GetPreferredUniqueNetId() : PlayerNetId;
	}
	return PlayerNetId;
}

bool UMultiplayerFunctionLibrary::IsClient(const UWorld* World)
{
	return World ? World->GetNetMode() >= NM_Client : false;
}

bool UMultiplayerFunctionLibrary::IsServer(const UWorld* World)
{
	return World ? World->GetNetMode() <= NM_ListenServer : true;
}

void UMultiplayerFunctionLibrary::LogNetMode(const UWorld* World)
{
	const FString LogString = World ? ToString(World->GetNetMode()) : "Invalid";
	UE_LOG(LogMultiplayer, Warning, TEXT("NetMode: %s"), *LogString);
}
