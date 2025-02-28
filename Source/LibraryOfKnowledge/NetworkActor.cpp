#include "NetworkActor.h"
#include "NetworkObject.h"
#include "Net/UnrealNetwork.h"

ANetworkActor::ANetworkActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ANetworkActor::PrintLog(const FString& LogString)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *LogString);
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, LogString);
}

void ANetworkActor::ClientReceiveNetworkObject_Implementation(UNetworkObject* InNetworkObject)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("%i"), InNetworkObject->Num));
}

void ANetworkActor::ServerPrintLog_Implementation(const FString& LogString)
{
	PrintLog(LogString);
}

void ANetworkActor::ClientPrintLog_Implementation(const FString& LogString)
{
	PrintLog(LogString);
}

void ANetworkActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetworkActor, NetworkObject);
}

void ANetworkActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UNetworkObject* NewNetworkObject = NewObject<UNetworkObject>();
		NewNetworkObject->Num = 4;
		//NetworkObject = NewNetworkObject;
		//ClientReceiveNetworkObject(NewNetworkObject);
		//ClientPrintLog(FString::Printf(TEXT("%i"), NetworkObject->Num));
	}
}
