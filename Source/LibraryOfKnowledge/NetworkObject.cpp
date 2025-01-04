#include "NetworkObject.h"
#include "Net/UnrealNetwork.h"

void UNetworkObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UNetworkObject, Num);
}
