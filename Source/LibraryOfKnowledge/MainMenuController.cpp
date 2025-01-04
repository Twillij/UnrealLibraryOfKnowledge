#include "MainMenuController.h"
#include "Blueprint/UserWidget.h"

void AMainMenuController::HostCampaignMode() const
{
	GetWorld()->ServerTravel(CampaignMap.GetLongPackageName() + "?listen");
}

void AMainMenuController::JoinCampaignMode(const FString& JoinURL)
{
	ClientTravel(JoinURL, TRAVEL_Absolute);
}

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
		SetShowMouseCursor(true);
	}
}
