#include "LoopingAudioPlayer.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"

ALoopingAudioPlayer::ALoopingAudioPlayer()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;

	bAutoPlay = true;
}

void ALoopingAudioPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoPlay)
	{
		RefreshAudio();
	}
}

void ALoopingAudioPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
	}

	Super::EndPlay(EndPlayReason);
}

void ALoopingAudioPlayer::RefreshAudio()
{
	if (!AudioComponent) return;

	AudioComponent->Stop();

	if (SoundToPlay)
	{
		AudioComponent->SetSound(SoundToPlay);
		
		if (AttenuationSettings)
		{
			AudioComponent->AttenuationSettings = AttenuationSettings;
		}

		AudioComponent->Play();
	}
}

#if WITH_EDITOR
void ALoopingAudioPlayer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALoopingAudioPlayer, SoundToPlay) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(ALoopingAudioPlayer, AttenuationSettings))
	{
		// Live preview changes in the editor viewport
		RefreshAudio();
	}
}
#endif
