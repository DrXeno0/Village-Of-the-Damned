#include "ObjectiveBlueprintLibrary.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

static UObjectiveSubsystem* GetObjectiveSubsystem(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		UWorld* World = WorldContextObject->GetWorld();
		if (World)
		{
			UGameInstance* GI = World->GetGameInstance();
			if (GI)
			{
				return GI->GetSubsystem<UObjectiveSubsystem>();
			}
		}
	}
	return nullptr;
}

void UObjectiveBlueprintLibrary::AddObjective(const UObject* WorldContextObject, FName ObjectiveID, FText Title, FText Description, bool bIsVisible)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->AddObjective(ObjectiveID, Title, Description, bIsVisible);
	}
}

void UObjectiveBlueprintLibrary::AddIncrementalObjective(const UObject* WorldContextObject, FName ObjectiveID, FText Title, FText Description, int32 TargetProgress, int32 StartProgress, bool bIsVisible)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->AddIncrementalObjective(ObjectiveID, Title, Description, TargetProgress, StartProgress, bIsVisible);
	}
}

void UObjectiveBlueprintLibrary::AdvanceObjectiveProgress(const UObject* WorldContextObject, FName ObjectiveID, int32 Amount)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->AdvanceObjectiveProgress(ObjectiveID, Amount);
	}
}

void UObjectiveBlueprintLibrary::SetObjectiveProgress(const UObject* WorldContextObject, FName ObjectiveID, int32 NewProgress)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->SetObjectiveProgress(ObjectiveID, NewProgress);
	}
}

void UObjectiveBlueprintLibrary::CompleteObjective(const UObject* WorldContextObject, FName ObjectiveID)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->CompleteObjective(ObjectiveID);
	}
}

void UObjectiveBlueprintLibrary::FailObjective(const UObject* WorldContextObject, FName ObjectiveID)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->FailObjective(ObjectiveID);
	}
}

void UObjectiveBlueprintLibrary::ResetObjectives(const UObject* WorldContextObject)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		Subsystem->ResetObjectives();
	}
}

TArray<FObjectiveInfo> UObjectiveBlueprintLibrary::GetActiveObjectives(const UObject* WorldContextObject)
{
	if (UObjectiveSubsystem* Subsystem = GetObjectiveSubsystem(WorldContextObject))
	{
		return Subsystem->GetActiveObjectives();
	}
	return TArray<FObjectiveInfo>();
}
