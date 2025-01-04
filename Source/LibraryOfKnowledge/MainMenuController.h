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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> CampaignMap;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MainMenuWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	void HostCampaignMode() const;

	UFUNCTION(BlueprintCallable)
	void JoinCampaignMode(const FString& JoinURL);
	
protected:
	virtual void BeginPlay() override;
};
