// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Factories/Factory.h>
#include "XD_GameEventGraphFactory.generated.h"

/**
 * 
 */
UCLASS()
class XD_GAMEEVENTSYSTEM_EDITOR_API UXD_GameEventGraphFactory : public UFactory
{
	GENERATED_BODY()

public:
	UXD_GameEventGraphFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	virtual FString GetDefaultNewAssetName() const override;
};
