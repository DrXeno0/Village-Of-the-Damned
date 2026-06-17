#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "ObjectiveSubsystem.generated.h"

class SObjectiveListWidget;

USTRUCT(BlueprintType)
struct FObjectiveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FName ObjectiveID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsFailed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsVisible = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsIncremental = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	int32 CurrentProgress = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	int32 TargetProgress = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveStatusChanged, FName, ObjectiveID);

UCLASS()
class VOD_API UObjectiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Lifecycle
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Blueprint Callable Functions
	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void AddObjective(FName ObjectiveID, FText Title, FText Description, bool bIsVisible = true);

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void AddIncrementalObjective(FName ObjectiveID, FText Title, FText Description, int32 TargetProgress, int32 StartProgress = 0, bool bIsVisible = true);

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void AdvanceObjectiveProgress(FName ObjectiveID, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void SetObjectiveProgress(FName ObjectiveID, int32 NewProgress);

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void CompleteObjective(FName ObjectiveID);

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void FailObjective(FName ObjectiveID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objectives")
	TArray<FObjectiveInfo> GetActiveObjectives() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Objectives")
	TArray<FObjectiveInfo> GetAllObjectives() const { return Objectives; }

	UFUNCTION(BlueprintCallable, Category = "Objectives")
	void ResetObjectives();

	// Blueprint Assignable Events
	UPROPERTY(BlueprintAssignable, Category = "Objectives")
	FOnObjectiveStatusChanged OnObjectiveAdded;

	UPROPERTY(BlueprintAssignable, Category = "Objectives")
	FOnObjectiveStatusChanged OnObjectiveCompleted;
	
	UPROPERTY(BlueprintAssignable, Category = "Objectives")
	FOnObjectiveStatusChanged OnObjectiveUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Objectives")
	FOnObjectiveStatusChanged OnObjectiveFailed;

private:
	UPROPERTY()
	TArray<FObjectiveInfo> Objectives;
};
