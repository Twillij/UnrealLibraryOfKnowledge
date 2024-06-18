#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSubsystem.generated.h"

UCLASS()
class MULTIPLAYER_API UMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSubsystem();
	
	FOnlineSessionSettings DefaultSessionSettings;
	FOnlineSessionSearch DefaultSessionSearch;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	FName OnlineSubsystemName = NAME_None;
	
	IOnlineSessionPtr SessionInterface = nullptr;
	TArray<FName> ActiveSessionNames;

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

public:
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FName SessionName);

	UFUNCTION(BlueprintCallable)
	void DestroySession(const FName SessionName);

	UFUNCTION(BlueprintCallable)
	void FindSessions();

	UFUNCTION(BlueprintCallable)
	void JoinSession(const FName SessionName);
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleted(bool bWasSuccessful);
};
