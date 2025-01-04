#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NetworkObject.generated.h"

UCLASS(Blueprintable)
class LIBRARYOFKNOWLEDGE_API UNetworkObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Num = 0;
	
protected:
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
