// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventGraphFactory.h"
#include "XD_GameEventGraph.h"

#define LOCTEXT_NAMESPACE "XD_GameEventGraph"

UXD_GameEventGraphFactory::UXD_GameEventGraphFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UXD_GameEventGraph::StaticClass();
}

UObject* UXD_GameEventGraphFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

FText UXD_GameEventGraphFactory::GetDisplayName() const
{
	return LOCTEXT("游戏事件图表", "游戏事件图表");
}

FString UXD_GameEventGraphFactory::GetDefaultNewAssetName() const
{
	return TEXT("游戏事件图表");
}

#undef LOCTEXT_NAMESPACE
