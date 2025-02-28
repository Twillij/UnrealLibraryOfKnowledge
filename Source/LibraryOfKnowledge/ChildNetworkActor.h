#pragma once

#include "NetworkActor.h"
#include "ChildNetworkActor.generated.h"

UCLASS()
class LIBRARYOFKNOWLEDGE_API AChildNetworkActor : public ANetworkActor
{
	GENERATED_BODY()

public:
	virtual void ServerPrintLog_Implementation(const FString& LogString) override;
	virtual void ClientPrintLog_Implementation(const FString& LogString) override;
};
