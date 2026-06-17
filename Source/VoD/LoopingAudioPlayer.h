#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LoopingAudioPlayer.generated.h"

class UAudioComponent;
class USoundBase;
class USoundAttenuation;

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent = true))
class VOD_API ALoopingAudioPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	ALoopingAudioPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	/** Root component to provide a transform in the level */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;

	/** The audio component that will play the sound */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

public:
	/** The Sound Wave or Sound Cue to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ExposeOnSpawn = "true"))
	USoundBase* SoundToPlay;

	/** Optional spatialization/attenuation settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundAttenuation* AttenuationSettings;

	/** If true, the sound will start playing immediately when spawned/loaded */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool bAutoPlay;

	/** Helper function to update/refresh the audio settings (used in-editor and during play) */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void RefreshAudio();
};
