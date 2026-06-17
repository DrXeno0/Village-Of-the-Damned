#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObjectiveSubsystem.h"
#include "ObjectiveBlueprintLibrary.generated.h"

UCLASS()
class VOD_API UObjectiveBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void AddObjective(const UObject* WorldContextObject, FName ObjectiveID, FText Title, FText Description, bool bIsVisible = true);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void AddIncrementalObjective(const UObject* WorldContextObject, FName ObjectiveID, FText Title, FText Description, int32 TargetProgress, int32 StartProgress = 0, bool bIsVisible = true);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void AdvanceObjectiveProgress(const UObject* WorldContextObject, FName ObjectiveID, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void SetObjectiveProgress(const UObject* WorldContextObject, FName ObjectiveID, int32 NewProgress);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void CompleteObjective(const UObject* WorldContextObject, FName ObjectiveID);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void FailObjective(const UObject* WorldContextObject, FName ObjectiveID);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static void ResetObjectives(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Objectives", meta = (WorldContext = "WorldContextObject"))
	static TArray<FObjectiveInfo> GetActiveObjectives(const UObject* WorldContextObject);
};
