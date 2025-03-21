#include "MyFunctionLibrary.h"

void UMyFunctionLibrary::ProcessStruct(const FMyStruct& MyStruct)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("Struct: %i"), MyStruct.Val));
}
