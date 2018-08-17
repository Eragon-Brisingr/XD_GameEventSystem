// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "XD_GameEventGraph_Impl.generated.h"

/**
 * 
 */
UCLASS(abstract)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphNode_GameEventSequenceBase : public UXD_GameEventGraphNode
{
	GENERATED_BODY()
public:
	UXD_GameEventGraphNode_GameEventSequenceBase();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "游戏事件", meta = (DisplayName = "游戏事件描述"))
	FText Describe;

	FText GetDescribe() const { return Describe; }
};

UCLASS(meta = (DisplayName = "游戏事件序列_分支型"))
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphNode_GameEventSequence_Branch : public UXD_GameEventGraphNode_GameEventSequenceBase
{
	GENERATED_BODY()
public:
	virtual bool CanCreateConnectionWithOtherNode_Implementation(UGenericGraphNode* Other, FText& ErrorMessage) const override;

	virtual void CheckNodeError_Implementation(EGenericGraphNodeCheckMessageType& GenericGraphNodeCheckMessageType, FString& Message, const FGenericGraphNodeErrorCollector& ErrorCollector) const override;

	virtual class UXD_GameEventSequenceBase* GetGameEventSequence(class UXD_GameEventBase* GameEvent, class UXD_GameEventGraphEdge* NextEdge) const override;
};

UCLASS(meta = (DisplayName = "游戏事件结束分支"))
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphNode_FinishBranch : public UXD_GameEventGraphNode
{
	GENERATED_BODY()
public:
	UXD_GameEventGraphNode_FinishBranch();

	virtual FText GetNodeTitle_Implementation() const override;

	virtual FText GetDescription_Implementation() const override;

	virtual bool CanCreateConnectionWithOtherNode_Implementation(UGenericGraphNode* Other, FText& ErrorMessage) const override;

	virtual TArray<FGenericGraph_NewNodeParamers> GetNodeTemplates_Implementation(UObject* Outer) const override;	
	
	virtual TArray<FGenericGraph_NewEdgeParamers> GetEdgeTemplates_Implementation(UObject* Outer) const override;

	virtual UObject* GetJumpTargetForDoubleClick_Implementation() const override;

	virtual class UXD_GameEventElementBase* GetMainGameEventElement() const override;
#if WITH_EDITOR
	virtual bool CanCreateEdgeNode() const override;
#endif
public:
	UPROPERTY(VisibleAnywhere, Category = "游戏事件", Instanced)
	class UXD_GameEventElementBase* GameEventElement;

	virtual class UXD_GameEventSequenceBase* GetGameEventSequence(class UXD_GameEventBase* GameEvent, class UXD_GameEventGraphEdge* NextEdge) const override;
};

UCLASS(meta = (DisplayName = "游戏事件元素"))
class XD_GAMEEVENTSYSTEM_API UXD_GameEventGraphDecorator_GameEventElement : public UXD_GameEventGraphDecorator
{
	GENERATED_BODY()
public:
	UXD_GameEventGraphDecorator_GameEventElement();

	virtual FText GetNodeTitle_Implementation() const override;

	virtual FText GetDescription_Implementation() const override;

	virtual TArray<FGenericGraph_NewDecoratorParamers> GetDecoratorTemplates_Implementation(UObject* Outer, const class UGenericGraphNodeWithDecorators* GraphNodeBase) const override;

	virtual UObject* GetJumpTargetForDoubleClick_Implementation() const override;
public:
	UPROPERTY(VisibleAnywhere, Category = "游戏事件", Instanced)
	class UXD_GameEventElementBase* GameEventElement;

	class UXD_GameEventElementBase* GetGameEventElementInstance(class UXD_GameEventSequenceBase* GameEventSequence) const;
};
