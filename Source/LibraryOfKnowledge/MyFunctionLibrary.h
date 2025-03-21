#pragma once

#include "CoreMinimal.h"
#include "MyGlobals.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyFunctionLibrary.generated.h"

UCLASS()
class LIBRARYOFKNOWLEDGE_API UMyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void ProcessStruct(const FMyStruct& MyStruct);
};
