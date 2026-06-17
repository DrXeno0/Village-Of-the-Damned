#include "ObjectiveSubsystem.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void UObjectiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UObjectiveSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UObjectiveSubsystem::AddObjective(FName ObjectiveID, FText Title, FText Description, bool bIsVisible)
{
	FObjectiveInfo* Existing = Objectives.FindByPredicate([ObjectiveID](const FObjectiveInfo& Obj) {
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (Existing)
	{
		Existing->Title = Title;
		Existing->Description = Description;
		Existing->bIsCompleted = false;
		Existing->bIsFailed = false;
		Existing->bIsVisible = bIsVisible;
		Existing->bIsIncremental = false;
		Existing->CurrentProgress = 0;
		Existing->TargetProgress = 0;
	}
	else
	{
		FObjectiveInfo NewObj;
		NewObj.ObjectiveID = ObjectiveID;
		NewObj.Title = Title;
		NewObj.Description = Description;
		NewObj.bIsCompleted = false;
		NewObj.bIsFailed = false;
		NewObj.bIsVisible = bIsVisible;
		NewObj.bIsIncremental = false;
		NewObj.CurrentProgress = 0;
		NewObj.TargetProgress = 0;
		Objectives.Add(NewObj);
	}

	OnObjectiveAdded.Broadcast(ObjectiveID);
}

void UObjectiveSubsystem::AddIncrementalObjective(FName ObjectiveID, FText Title, FText Description, int32 TargetProgress, int32 StartProgress, bool bIsVisible)
{
	FObjectiveInfo* Existing = Objectives.FindByPredicate([ObjectiveID](const FObjectiveInfo& Obj) {
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (Existing)
	{
		Existing->Title = Title;
		Existing->Description = Description;
		Existing->bIsCompleted = false;
		Existing->bIsFailed = false;
		Existing->bIsVisible = bIsVisible;
		Existing->bIsIncremental = true;
		Existing->CurrentProgress = FMath::Clamp(Existing->CurrentProgress, 0, TargetProgress);
		Existing->TargetProgress = TargetProgress;
	}
	else
	{
		FObjectiveInfo NewObj;
		NewObj.ObjectiveID = ObjectiveID;
		NewObj.Title = Title;
		NewObj.Description = Description;
		NewObj.bIsCompleted = false;
		NewObj.bIsFailed = false;
		NewObj.bIsVisible = bIsVisible;
		NewObj.bIsIncremental = true;
		NewObj.CurrentProgress = FMath::Clamp(StartProgress, 0, TargetProgress);
		NewObj.TargetProgress = TargetProgress;
		Objectives.Add(NewObj);
	}

	OnObjectiveAdded.Broadcast(ObjectiveID);
}

void UObjectiveSubsystem::AdvanceObjectiveProgress(FName ObjectiveID, int32 Amount)
{
	FObjectiveInfo* Existing = Objectives.FindByPredicate([ObjectiveID](const FObjectiveInfo& Obj) {
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (Existing && Existing->bIsIncremental && !Existing->bIsCompleted && !Existing->bIsFailed)
	{
		SetObjectiveProgress(ObjectiveID, Existing->CurrentProgress + Amount);
		OnObjectiveUpdated.Broadcast(ObjectiveID);
	}
}

void UObjectiveSubsystem::SetObjectiveProgress(FName ObjectiveID, int32 NewProgress)
{
	FObjectiveInfo* Existing = Objectives.FindByPredicate([ObjectiveID](const FObjectiveInfo& Obj) {
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (Existing && Existing->bIsIncremental && !Existing->bIsCompleted && !Existing->bIsFailed)
	{
		Existing->CurrentProgress = FMath::Clamp(NewProgress, 0, Existing->TargetProgress);
		
		if (Existing->CurrentProgress >= Existing->TargetProgress)
		{
			CompleteObjective(ObjectiveID);
		}
		else
		{
			OnObjectiveAdded.Broadcast(ObjectiveID);
		}
	}
}

void UObjectiveSubsystem::CompleteObjective(FName ObjectiveID)
{
	FObjectiveInfo* Existing = Objectives.FindByPredicate([ObjectiveID](const FObjectiveInfo& Obj) {
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (Existing && !Existing->bIsCompleted)
	{
		Existing->bIsCompleted = true;
		OnObjectiveCompleted.Broadcast(ObjectiveID);
	}
}

void UObjectiveSubsystem::FailObjective(FName ObjectiveID)
{
	FObjectiveInfo* Existing = Objectives.FindByPredicate([ObjectiveID](const FObjectiveInfo& Obj) {
		return Obj.ObjectiveID == ObjectiveID;
	});

	if (Existing && !Existing->bIsFailed)
	{
		Existing->bIsFailed = true;
		OnObjectiveFailed.Broadcast(ObjectiveID);
	}
}

TArray<FObjectiveInfo> UObjectiveSubsystem::GetActiveObjectives() const
{
	TArray<FObjectiveInfo> Active;
	for (const FObjectiveInfo& Obj : Objectives)
	{
		if (!Obj.bIsCompleted && !Obj.bIsFailed && Obj.bIsVisible)
		{
			Active.Add(Obj);
		}
	}
	return Active;
}

void UObjectiveSubsystem::ResetObjectives()
{
	Objectives.Empty();
}
