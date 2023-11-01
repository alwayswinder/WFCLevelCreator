// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCGridManager.h"
#include "K2Node_SpawnActor.h"
#include "WFCItemBase.h"
#include "WFCRolesManager.h"

AWFCGridManager::AWFCGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWFCGridManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWFCGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWFCGridManager::SetGridItemsHidenInEditor(bool IsHid)
{
	
}




