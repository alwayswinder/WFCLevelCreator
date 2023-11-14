// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WFCItemBase.generated.h"


//简单的连通性
UENUM(BlueprintType)
enum class EWFCLinkType : uint8
{
	Type_1 = 0,//
	Type_2 = 1,//
	Type_3 = 2,
	Type_4 = 3,
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC item")
	TObjectPtr<UTexture2D> ShowIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC item")
	float Probability = 1.0;
	
	//0-right,1-back, 2-left, 3-front
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WFC item")
	TArray<EWFCLinkType> LinkTypes = {EWFCLinkType::Type_1, EWFCLinkType::Type_1, EWFCLinkType::Type_1,EWFCLinkType::Type_1};

	
};
