#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultiplayerFunctionLibrary.generated.h"

UCLASS()
class MULTIPLAYER_API UMultiplayerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the unique net ID of the first local player from the world's controller.
	static FUniqueNetIdRepl GetLocalPlayerUniqueNetId(const UWorld* World);

	// Logs the current net mode.
	static void LogNetMode(const UWorld* World);
};
