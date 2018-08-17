// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericGraph.h"
#include "GenericGraphNode.h"
#include "GenericGraphDecorator.h"
#include "XD_GameEventGraph.generated.h"

/**
 * 
 */
UCLASS()
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraph : public UGenericGraph
{
	GENERATED_BODY()
	
public:
	UXD_GameEventGraph();
	
	virtual FText GetGraphName_Implementation() const;

#if WITH_EDITOR
	virtual void RebuildGenericGraph() override;
#endif

public:
	UPROPERTY()
	class UXD_GameEventGraphNode* FirstGameEventGraphNode;

	UPROPERTY(EditAnywhere, Category = "游戏事件")
	FText GameEventName;
};

UCLASS(meta = (DisplayName = "游戏事件开始"))
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphRootNode : public UGenericGraphNode
{
	GENERATED_BODY()
public:
	UXD_GameEventGraphRootNode();

	virtual FText GetDescription_Implementation() const override;

};

UCLASS(abstract)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphNode : public UGenericGraphNode
{
	GENERATED_BODY()

public:
	UXD_GameEventGraphNode();

	virtual class UXD_GameEventSequenceBase* GetGameEventSequence(class UXD_GameEventBase* GameEvent, class UXD_GameEventGraphEdge* NextEdge) const { return nullptr; }

	//返回主导的游戏事件元素，一般是用作分支条件的游戏事件元素
	virtual class UXD_GameEventElementBase* GetMainGameEventElement() const { return nullptr; }
};

UCLASS(abstract)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphEdge : public UGenericGraphEdge
{
	GENERATED_BODY()

public:


};

UCLASS(abstract)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphDecorator : public UGenericGraphDecorator
{
	GENERATED_BODY()

public:
	UXD_GameEventGraphDecorator();

};
