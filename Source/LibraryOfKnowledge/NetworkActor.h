#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkActor.generated.h"

class UNetworkObject;

UCLASS()
class LIBRARYOFKNOWLEDGE_API ANetworkActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	UNetworkObject* NetworkObject = nullptr;
	
public:
	ANetworkActor();

	UFUNCTION(BlueprintCallable)
	static void PrintLog(const FString& LogString);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerPrintLog(const FString& LogString);

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientPrintLog(const FString& LogString);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
};
