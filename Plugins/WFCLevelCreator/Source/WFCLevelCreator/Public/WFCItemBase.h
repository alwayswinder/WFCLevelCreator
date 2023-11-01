// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCItemBase.generated.h"

UCLASS()
class WFCLEVELCREATOR_API AWFCNextDebugBase : public AActor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "WFC Debug")
	void ShowDebug();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "WFC Debug")
	void RemoveDebug();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Debug")
	TArray<TSubclassOf<AWFCItemBase>> ClassesDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Debug")
	TArray<int32> RotDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC Debug")
	int32 GridIndex = 0;
};


UCLASS()
class WFCLEVELCREATOR_API AWFCItemBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWFCItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override; 
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "WFC Item")
	bool CheckNeighborAdapt(AWFCItemBase *Neighbor);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "WFC Item")
	void AutoAlign();

	
};
