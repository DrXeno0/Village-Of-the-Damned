#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VodGameInstance.generated.h"

class UTexture2D;
struct FSlateBrush;

UCLASS()
class VOD_API UVodGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UVodGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

private:
	/** Shows a plain black screen during any level load. */
	void HandleBeginLoadingScreen(const FString& MapName);

	/** After the village level loads, show the tutorial overlay. */
	void HandleEndLoadingScreen(UWorld* LoadedWorld);

	/** Displays the in-game tutorial controls overlay. */
	void ShowGameplayTutorial();

	/** Removes the tutorial overlay from the viewport (called by timer). */
	UFUNCTION()
	void RemoveTutorialOverlay();

	/** Returns current tutorial opacity for Slate animation. */
	FSlateColor GetTutorialOpacity() const;

	UPROPERTY()
	UTexture2D* TutorialTexture;

	TSharedPtr<FSlateBrush>    TutorialBrush;
	TSharedPtr<SWidget>        TutorialWidget;

	FTimerHandle               TutorialRemoveTimer;
	double                     TutorialShowTime;
};
