#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMultiplayer, Display, All)

UCLASS()
class MULTIPLAYER_API UMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	FOnlineSessionSettings DefaultSessionSettings;
	FOnlineSessionSearch DefaultSessionSearch;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	FName OnlineSubsystemName = NAME_None;
	
	IOnlineSessionPtr SessionInterface = nullptr;
	TArray<FName> CurrentSessionNames;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

public:
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName& SessionName);

	UFUNCTION(BlueprintCallable)
	void DestroySession(const FName& SessionName);

	UFUNCTION(BlueprintCallable)
	void FindSession(const FName SessionName);

	UFUNCTION(BlueprintCallable)
	void JoinSession(const FName SessionName);
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override {}
};
