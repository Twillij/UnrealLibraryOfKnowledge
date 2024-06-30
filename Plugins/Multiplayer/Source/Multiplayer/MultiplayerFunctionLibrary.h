#pragma once

#include "CoreMinimal.h"
#include "MultiplayerGlobals.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultiplayerFunctionLibrary.generated.h"

UCLASS()
class MULTIPLAYER_API UMultiplayerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the unique net ID of the first local player from the world's controller.
	UFUNCTION(BlueprintPure)
	static FUniqueNetIdRepl GetLocalPlayerUniqueNetId(const UWorld* World);

	// Returns the session info that was extracted from the session search result.
	UFUNCTION(BlueprintPure)
	static FMultiplayerSessionInfo GetSessionInfoFromSearchResult(const FMultiplayerSessionSearchResult& SessionSearchResult);

	// Returns true if the world is running on a client build. Defaults false if the world is invalid.
	UFUNCTION(BlueprintPure)
	static bool IsClient(const UWorld* World);

	// Returns true if the world is running on a server build. Defaults true if the world is invalid.
	UFUNCTION(BlueprintPure)
	static bool IsServer(const UWorld* World);
	
	// Logs the current net mode.
	UFUNCTION(BlueprintCallable)
	static void LogNetMode(const UWorld* World);
};
