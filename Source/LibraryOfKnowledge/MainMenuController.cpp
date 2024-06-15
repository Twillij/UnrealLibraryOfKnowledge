#include "MainMenuController.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		MainMenuWidget = Cast<UUserWidget>(CreateWidget(this, MainMenuWidgetClass));
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
		}
	}
}
