#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

UCLASS()
class LIBRARYOFKNOWLEDGE_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainMenuWidgetClass = nullptr;

protected:
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MainMenuWidget = nullptr;
	
protected:
	virtual void BeginPlay() override;
};
