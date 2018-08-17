// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventGraph.h"


#define LOCTEXT_NAMESPACE "游戏事件图表"

UXD_GameEventGraph::UXD_GameEventGraph()
{
#if WITH_EDITORONLY_DATA
	NodeType = UXD_GameEventGraphNode::StaticClass();
	RootNodeType = UXD_GameEventGraphRootNode::StaticClass();
#endif
}

FText UXD_GameEventGraph::GetGraphName_Implementation() const
{
	return LOCTEXT("游戏事件图表", "游戏事件图表");
}

#if WITH_EDITOR
void UXD_GameEventGraph::RebuildGenericGraph()
{
	FirstGameEventGraphNode = FindRootLinkedNode<UXD_GameEventGraphNode>();
}
#endif

UXD_GameEventGraphRootNode::UXD_GameEventGraphRootNode()
{
#if WITH_EDITORONLY_DATA
	bCanShowInGraphContext = false;
	bCanUserDelete = false;
	bCreateEdgeNode = false;
#endif
}

FText UXD_GameEventGraphRootNode::GetDescription_Implementation() const
{
	return LOCTEXT("游戏事件开始", "游戏事件开始");
}

UXD_GameEventGraphNode::UXD_GameEventGraphNode()
{
#if WITH_EDITORONLY_DATA
	bCreateEdgeNode = false;
#endif
}

UXD_GameEventGraphDecorator::UXD_GameEventGraphDecorator()
{
#if WITH_EDITORONLY_DATA
	DecorateGenericGraphType = UXD_GameEventGraph::StaticClass();
#endif
}

#undef LOCTEXT_NAMESPACE

