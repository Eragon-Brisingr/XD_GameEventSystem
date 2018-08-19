// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventGraph_Impl.h"
#include "XD_ObjectFunctionLibrary.h"


#define LOCTEXT_NAMESPACE "GameEventGraph"

const FColor GameEventSequenceColor = FLinearColor(0.1f, 0.1f, 0.5f, 1.f).ToFColor(true);
const FColor GameEventFinishBranchColor = FLinearColor(0.5f, 0.1f, 0.1f, 1.f).ToFColor(true);

UXD_GameEventGraphNode_GameEventSequenceBase::UXD_GameEventGraphNode_GameEventSequenceBase()
{
#if WITH_EDITORONLY_DATA
	bMultiEdgeWithoutNode = true;
	BackgroundColor = GameEventSequenceColor;
	CategoryName = LOCTEXT("游戏事件序列", "游戏事件序列");
	SpecialDecoratorTypes.Add(UXD_GameEventGraphDecorator_GameEventElement::StaticClass());
	Describe = LOCTEXT("游戏事件描述_未填写", "游戏事件描述_未填写");
#endif
}

bool UXD_GameEventGraphNode_GameEventSequence_Branch::CanCreateConnectionWithOtherNode_Implementation(UGenericGraphNode* Other, FText& ErrorMessage) const
{
	if (Other->IsA<UXD_GameEventGraphNode_FinishBranch>())
	{
		return true;
	}
	ErrorMessage = LOCTEXT("只可连接游戏事件结束分支", "只可连接游戏事件结束分支");
	return false;
}

void UXD_GameEventGraphNode_GameEventSequence_Branch::CheckNodeError_Implementation(EGenericGraphNodeCheckMessageType& GenericGraphNodeCheckMessageType, FString& Message, const FGenericGraphNodeErrorCollector& ErrorCollector) const
{
	if (ChildrenNodes.Num() == 0)
	{
		GenericGraphNodeCheckMessageType = EGenericGraphNodeCheckMessageType::Error;
		Message = TEXT("必须连接游戏事件结束分支");
	}
}

class UXD_GameEventSequenceBase* UXD_GameEventGraphNode_GameEventSequence_Branch::GetGameEventSequence(class UXD_GameEventBase* GameEvent, class UXD_GameEventGraphEdge* NextEdge) const
{
	UGameEventSequence_Branch* GameEventSequence = NewObject<UGameEventSequence_Branch>(GameEvent);
	GameEventSequence->GameEventSequenceTemplate = this;
	GameEventSequence->OwingGameEvent = GameEvent;

	for (UXD_GameEventGraphDecorator_GameEventElement* Decorator_GameEventElement : GetDecoratorsByType<UXD_GameEventGraphDecorator_GameEventElement>())
	{
		if (Decorator_GameEventElement->GameEventElement)
		{
			GameEventSequence->GameEventElementList.Add(Decorator_GameEventElement->GetGameEventElementInstance(GameEventSequence));
		}
	}

	for (UXD_GameEventGraphNode_FinishBranch* Node_FinishBranch : GetChildrenNodesByType<UXD_GameEventGraphNode_FinishBranch>())
	{
		FGameEventElementFinishWarpper GameEventElementFinishWarpper;
		GameEventElementFinishWarpper.GameEventFinishBranch = Node_FinishBranch;
		UXD_GameEventElementBase* GameEventElementInstance = UXD_ObjectFunctionLibrary::DuplicateObject(Node_FinishBranch->GameEventElement, GameEventSequence);
		GameEventElementInstance->OwingGameEventSequence = GameEventSequence;
		GameEventElementFinishWarpper.GameEventElement = GameEventElementInstance;

		GameEventSequence->GameEventElementFinishList.Add(GameEventElementFinishWarpper);
	}

	return GameEventSequence;
}

UXD_GameEventGraphNode_FinishBranch::UXD_GameEventGraphNode_FinishBranch()
{
#if WITH_EDITORONLY_DATA
	BackgroundColor = GameEventFinishBranchColor;
	bCreateEdgeNode = true;
#endif
}

FText UXD_GameEventGraphNode_FinishBranch::GetNodeTitle_Implementation() const
{
	FString TitleName = UXD_ObjectFunctionLibrary::GetClassName(GetClass());
	if (GameEventElement)
	{
		TitleName = UXD_ObjectFunctionLibrary::GetClassName(GameEventElement->GetClass()) + TEXT("_") + TitleName;
	}
	return FText::FromString(TitleName);
}

FText UXD_GameEventGraphNode_FinishBranch::GetDescription_Implementation() const
{
	return FText::FromString(UXD_ObjectFunctionLibrary::GetObjectPropertysDesc(GameEventElement, UObject::StaticClass()));
}

bool UXD_GameEventGraphNode_FinishBranch::CanCreateConnectionWithOtherNode_Implementation(UGenericGraphNode* Other, FText& ErrorMessage) const
{
	if (Other->IsA<UXD_GameEventGraphNode_GameEventSequenceBase>())
	{
		return true;
	}
	ErrorMessage = LOCTEXT("只可连接游戏事件序列", "只可连接游戏事件序列");
	return false;
}

TArray<FGenericGraph_NewNodeParamers> UXD_GameEventGraphNode_FinishBranch::GetNodeTemplates_Implementation(UObject* Outer) const
{
	TArray<FGenericGraph_NewNodeParamers> Res;
	for (TSubclassOf<UXD_GameEventElementBase>& GameEventElementClass : UXD_ObjectFunctionLibrary::GetAllSubclass<UXD_GameEventElementBase>())
	{
		FGenericGraph_NewNodeParamers NewNodeParamers;
		UXD_GameEventGraphNode_FinishBranch* FinishBranch = NewObject<UXD_GameEventGraphNode_FinishBranch>(Outer);
		FinishBranch->GameEventElement = NewObject<UXD_GameEventElementBase>(FinishBranch, GameEventElementClass);
		FinishBranch->GameEventElement->SetTemplate(FinishBranch);
		NewNodeParamers.InMenuDesc = FText::FromString(UXD_ObjectFunctionLibrary::GetClassName(GameEventElementClass));
		NewNodeParamers.InNodeCategory = LOCTEXT("结束分支", "结束分支");
		NewNodeParamers.NodeTemplate = FinishBranch;
		Res.Add(NewNodeParamers);
	}
	return Res;
}

TArray<FGenericGraph_NewEdgeParamers> UXD_GameEventGraphNode_FinishBranch::GetEdgeTemplates_Implementation(UObject* Outer) const
{
	return GameEventElement ? GameEventElement->GetFinishGameEventElementBranchEdgeTemplates(Outer, this) : TArray<FGenericGraph_NewEdgeParamers>();
}

UObject* UXD_GameEventGraphNode_FinishBranch::GetJumpTargetForDoubleClick_Implementation() const
{
	return GameEventElement ? GameEventElement->GetJumpTargetForDoubleClick() : nullptr;
}

class UXD_GameEventElementBase* UXD_GameEventGraphNode_FinishBranch::GetMainGameEventElement() const
{
	return GameEventElement;
}

#if WITH_EDITOR
bool UXD_GameEventGraphNode_FinishBranch::CanCreateEdgeNode() const
{
	return GameEventElement ? GameEventElement->bHasMultiBranch : false;
}
#endif

class UXD_GameEventSequenceBase* UXD_GameEventGraphNode_FinishBranch::GetGameEventSequence(class UXD_GameEventBase* GameEvent, class UXD_GameEventGraphEdge* NextEdge) const
{
	return GetNodeByEdge<UXD_GameEventGraphNode>(NextEdge)->GetGameEventSequence(GameEvent, nullptr);
}

bool UXD_GameEventGraphNode_GameEventSequence_List::CanCreateConnectionWithOtherNode_Implementation(UGenericGraphNode* Other, FText& ErrorMessage) const
{
	if (Other->IsA<UXD_GameEventGraphNode_FinishBranch>())
	{
		ErrorMessage = LOCTEXT("不可连接游戏事件结束分支", "不可连接游戏事件结束分支");
		return false;
	}
	else if (ChildrenNodes.Num() >= 1)
	{
		ErrorMessage = LOCTEXT("只能连接一个游戏事件序列", "只能连接一个游戏事件序列");
		return false;
	}
	return true;
}

void UXD_GameEventGraphNode_GameEventSequence_List::CheckNodeError_Implementation(EGenericGraphNodeCheckMessageType& GenericGraphNodeCheckMessageType, FString& Message, const FGenericGraphNodeErrorCollector& ErrorCollector) const
{
	if (Decorators.Num() == 0)
	{
		GenericGraphNodeCheckMessageType = EGenericGraphNodeCheckMessageType::Error;
		Message = TEXT("必须存在游戏事件元素");
	}
}

class UXD_GameEventSequenceBase* UXD_GameEventGraphNode_GameEventSequence_List::GetGameEventSequence(class UXD_GameEventBase* GameEvent, class UXD_GameEventGraphEdge* NextEdge) const
{
	UGameEventSequence_List* GameEventSequence = NewObject<UGameEventSequence_List>(GameEvent);
	GameEventSequence->GameEventSequenceTemplate = this;
	GameEventSequence->OwingGameEvent = GameEvent;

	GameEventSequence->NextGameEvent = ChildrenNodes.Num() > 0 ? Cast<UXD_GameEventGraphNode>(ChildrenNodes[0]) : nullptr;

	for (UXD_GameEventGraphDecorator_GameEventElement* Decorator_GameEventElement : GetDecoratorsByType<UXD_GameEventGraphDecorator_GameEventElement>())
	{
		if (Decorator_GameEventElement->GameEventElement)
		{
			GameEventSequence->GameEventElementList.Add(Decorator_GameEventElement->GetGameEventElementInstance(GameEventSequence));
		}
	}

	return GameEventSequence;
}

UXD_GameEventGraphDecorator_GameEventElement::UXD_GameEventGraphDecorator_GameEventElement()
{
#if WITH_EDITORONLY_DATA
	bForNode = false;
	bAllowMulti = true;
#endif
}

FText UXD_GameEventGraphDecorator_GameEventElement::GetNodeTitle_Implementation() const
{
	FString TitleName = UXD_ObjectFunctionLibrary::GetClassName(GetClass());
	if (GameEventElement)
	{
		TitleName = UXD_ObjectFunctionLibrary::GetClassName(GameEventElement->GetClass()) + TEXT("_") + TitleName;
	}
	return FText::FromString(TitleName);
}

FText UXD_GameEventGraphDecorator_GameEventElement::GetDescription_Implementation() const
{
	return FText::FromString(UXD_ObjectFunctionLibrary::GetObjectPropertysDesc(GameEventElement, UObject::StaticClass()));
}

TArray<FGenericGraph_NewDecoratorParamers> UXD_GameEventGraphDecorator_GameEventElement::GetDecoratorTemplates_Implementation(UObject* Outer, const class UGenericGraphNodeWithDecorators* GraphNodeBase) const
{
	TArray<FGenericGraph_NewDecoratorParamers> Res;
	for (TSubclassOf<UXD_GameEventElementBase>& GameEventElementClass : UXD_ObjectFunctionLibrary::GetAllSubclass<UXD_GameEventElementBase>())
	{
		FGenericGraph_NewDecoratorParamers NewDecoratorParamers;
		UXD_GameEventGraphDecorator_GameEventElement* GameEventGraphDecorator_GameEventElement = NewObject<UXD_GameEventGraphDecorator_GameEventElement>(Outer);
		GameEventGraphDecorator_GameEventElement->GameEventElement = NewObject<UXD_GameEventElementBase>(GameEventGraphDecorator_GameEventElement, GameEventElementClass);
		GameEventGraphDecorator_GameEventElement->GameEventElement->SetTemplate(GameEventGraphDecorator_GameEventElement);
		NewDecoratorParamers.InMenuDesc = FText::FromString(UXD_ObjectFunctionLibrary::GetClassName(GameEventElementClass));
		NewDecoratorParamers.DecoratorTemplate = GameEventGraphDecorator_GameEventElement;
		Res.Add(NewDecoratorParamers);
	}
	return Res;
}

UObject* UXD_GameEventGraphDecorator_GameEventElement::GetJumpTargetForDoubleClick_Implementation() const
{
	return GameEventElement ? GameEventElement->GetJumpTargetForDoubleClick() : nullptr;
}

class UXD_GameEventElementBase* UXD_GameEventGraphDecorator_GameEventElement::GetGameEventElementInstance(class UXD_GameEventSequenceBase* GameEventSequence) const
{
	UXD_GameEventElementBase* GameEventElementInstance = UXD_ObjectFunctionLibrary::DuplicateObject(GameEventElement, GameEventSequence);
	GameEventElementInstance->OwingGameEventSequence = GameEventSequence;
	return GameEventElementInstance;
}

#undef LOCTEXT_NAMESPACE

