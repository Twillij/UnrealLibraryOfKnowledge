#include "ChildNetworkActor.h"

void AChildNetworkActor::ServerPrintLog_Implementation(const FString& LogString)
{
	PrintLog("Child Override: Server");
}

void AChildNetworkActor::ClientPrintLog_Implementation(const FString& LogString)
{
	PrintLog("Child Override: Client");
}
