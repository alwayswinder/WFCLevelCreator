// Fill out your copyright notice in the Description page of Project Settings.


#include "WFCRolesManager.h"
#include "LevelCreatorLibrary.h"
#include "WFCItemBase.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "UObject/ConstructorHelpers.h"

UWFCRolesManagerAsset::UWFCRolesManagerAsset()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint>FindObject(TEXT("/Script/Engine.Blueprint'/WFCLevelCreator/WFC/Core/BP_WFCGridManager.BP_WFCGridManager'"));
	if(FindObject.Object)
	{
		WFCGridManagerClass=(UClass*)FindObject.Object->GeneratedClass;
	}
}

void UWFCRolesManagerAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}







