// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LevelCreatorLibrary.generated.h"

/**
 * 
 */
UCLASS()
class WFCLEVELCREATOR_API ULevelCreatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "Add Datatable Row", DefaultToSelf = "Object",
		 CustomStructureParam = "CustomStruct", AdvancedDisplay = "Object"), Category = "DataTable")
	static bool AddRowDT(int32 CustomStruct, UDataTable* DT, FName NewRowName);
	static bool Generic_AddRowDT(void* Target, FProperty* Pro, UDataTable* DT, FName NewRowName);
	DECLARE_FUNCTION(execAddRowDT)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(NULL);
		void* AAddr = Stack.MostRecentPropertyAddress;
		FProperty* Property = CastField<FProperty>(Stack.MostRecentProperty);
		P_GET_OBJECT(UDataTable, DT);
		P_GET_PROPERTY(FNameProperty, NewRowName);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result = Generic_AddRowDT(AAddr, Property, DT, NewRowName);
		P_NATIVE_END;
	}
	UFUNCTION(BlueprintCallable)
	static void GetObjThumbnail(int32 ImageRes, UObject *InObject, UTexture2D* InTexture2d);
};
