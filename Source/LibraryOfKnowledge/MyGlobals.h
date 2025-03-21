#pragma once

#include "CoreMinimal.h"
#include "MyGlobals.generated.h"

USTRUCT(BlueprintType)
struct FMyStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int Val = 0;

	//friend FArchive& operator<<(FArchive& Archive, FMyStruct& MyStruct) override;
};

USTRUCT(BlueprintType)
struct FMyChildStruct : public FMyStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int ChildVal = 0;
	
	FMyChildStruct() { Val = 1; }
};
