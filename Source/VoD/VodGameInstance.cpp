#include "VodGameInstance.h"

#include "MoviePlayer.h"
#include "ContentStreaming.h"
#include "ImageUtils.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"

#include "Brushes/SlateImageBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SOverlay.h"

// ─────────────────────────────────────────────────────────────────────────────
// How long the tutorial is visible on-screen (seconds)
static constexpr float TutorialHoldDuration   = 8.0f;
static constexpr float TutorialFadeInDuration  = 0.5f;
static constexpr float TutorialFadeOutDuration = 2.0f;
static constexpr float TutorialTotalDuration   = TutorialFadeInDuration + TutorialHoldDuration + TutorialFadeOutDuration;

// ─────────────────────────────────────────────────────────────────────────────

UVodGameInstance::UVodGameInstance()
	: TutorialTexture(nullptr)
	, TutorialShowTime(0.0)
{
}

void UVodGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(
		this, &UVodGameInstance::HandleBeginLoadingScreen);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(
		this, &UVodGameInstance::HandleEndLoadingScreen);
}

void UVodGameInstance::Shutdown()
{
	// Clean up any pending timer / overlay before shutdown
	RemoveTutorialOverlay();

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	Super::Shutdown();
}

// ─────────────────────────────────────────────────────────────────────────────
// LOADING SCREEN — plain black, blocks until level is fully loaded

void UVodGameInstance::HandleBeginLoadingScreen(const FString& MapName)
{
	if (IsRunningDedicatedServer()) return;

	FLoadingScreenAttributes Screen;
	Screen.bAutoCompleteWhenLoadingCompletes = true;
	Screen.MinimumLoadingScreenDisplayTime   = 0.0f;
	Screen.WidgetLoadingScreen =
		SNew(SColorBlock).Color(FLinearColor::Black);

	GetMoviePlayer()->SetupLoadingScreen(Screen);
}

void UVodGameInstance::HandleEndLoadingScreen(UWorld* LoadedWorld)
{
	if (IsRunningDedicatedServer() || !LoadedWorld) return;

	// Block until streaming assets are ready so the level looks good immediately
	IStreamingManager::Get().StreamAllResources(5.0f);

	// Only show the tutorial on the village gameplay level
	const FString MapName = LoadedWorld->GetMapName();
	if (MapName.Contains(TEXT("Lvl_Village")))
	{
		ShowGameplayTutorial();
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// IN-GAME TUTORIAL OVERLAY

void UVodGameInstance::ShowGameplayTutorial()
{
	UGameViewportClient* Viewport = GetGameViewportClient();
	if (!Viewport) return;

	// Load the tutorial texture — try raw PNG on disk first (so updates are immediate), then fall back to UAsset
	const FString RawPath = FPaths::ProjectContentDir() / TEXT("UI/Images/tutorial_screen.png");
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*RawPath))
	{
		TutorialTexture = FImageUtils::ImportFileAsTexture2D(RawPath);
	}

	if (!TutorialTexture)
	{
		TutorialTexture = LoadObject<UTexture2D>(
			nullptr,
			TEXT("/Game/UI/Images/tutorial_screen.tutorial_screen")
		);
	}
	if (!TutorialTexture) return;

	TutorialBrush = MakeShareable(new FSlateImageBrush(
		TutorialTexture,
		FVector2D(static_cast<float>(TutorialTexture->GetSizeX()),
		          static_cast<float>(TutorialTexture->GetSizeY()))
	));

	TutorialShowTime = FPlatformTime::Seconds();

	TutorialWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(700.f)
			.HeightOverride(700.f)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFit)
				[
					SNew(SImage)
					.Image(TutorialBrush.Get())
					.ColorAndOpacity_UObject(this, &UVodGameInstance::GetTutorialOpacity)
				]
			]
		];

	Viewport->AddViewportWidgetContent(TutorialWidget.ToSharedRef(), /*ZOrder=*/ 10);

	// Schedule removal after the full animation completes
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TutorialRemoveTimer,
			this,
			&UVodGameInstance::RemoveTutorialOverlay,
			TutorialTotalDuration,
			/*bLoop=*/ false
		);
	}
}

void UVodGameInstance::RemoveTutorialOverlay()
{
	if (TutorialWidget.IsValid())
	{
		if (UGameViewportClient* Viewport = GetGameViewportClient())
		{
			Viewport->RemoveViewportWidgetContent(TutorialWidget.ToSharedRef());
		}
		TutorialWidget.Reset();
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TutorialRemoveTimer);
	}

	TutorialBrush.Reset();
}

// ─────────────────────────────────────────────────────────────────────────────
// FADE ANIMATION — called by Slate every frame to animate opacity

FSlateColor UVodGameInstance::GetTutorialOpacity() const
{
	const double Elapsed = FPlatformTime::Seconds() - TutorialShowTime;
	float Alpha = 0.0f;

	if (Elapsed < TutorialFadeInDuration)
	{
		// Fade in
		Alpha = static_cast<float>(Elapsed / TutorialFadeInDuration);
	}
	else if (Elapsed < TutorialFadeInDuration + TutorialHoldDuration)
	{
		// Hold at full opacity
		Alpha = 1.0f;
	}
	else if (Elapsed < TutorialTotalDuration)
	{
		// Fade out
		const double FadeElapsed = Elapsed - TutorialFadeInDuration - TutorialHoldDuration;
		Alpha = static_cast<float>(1.0 - FadeElapsed / TutorialFadeOutDuration);
	}

	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, FMath::Clamp(Alpha, 0.0f, 1.0f)));
}
