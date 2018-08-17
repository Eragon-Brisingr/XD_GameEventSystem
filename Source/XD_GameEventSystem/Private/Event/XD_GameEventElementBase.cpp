// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventElementBase.h"
#include "XD_DebugFunctionLibrary.h"


#define LOCTEXT_NAMESPACE "游戏事件"

UXD_GameEventElementBase::UXD_GameEventElementBase()
	:bIsMust(true), bIsShowGameEventElement(true), bIsFinish(false)//, bIsActive(true)
{

}

class UWorld* UXD_GameEventElementBase::GetWorld() const
{
	return GetGameEvent() ? GetGameEvent()->GetWorld() : nullptr;
}

bool UXD_GameEventElementBase::IsSupportedForNetworking() const
{
	return true;
}

void UXD_GameEventElementBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}

	DOREPLIFETIME(UXD_GameEventElementBase, bIsFinish);
	DOREPLIFETIME(UXD_GameEventElementBase, bIsMust);
	DOREPLIFETIME(UXD_GameEventElementBase, bIsShowGameEventElement);

}

FText UXD_GameEventElementBase::GetDescribe()
{
	return bIsMust ? ReceiveGetDescribe() : FText::Format(LOCTEXT("GameEventElementFormat", "{0} (可选)"), ReceiveGetDescribe());
}

FText UXD_GameEventElementBase::ReceiveGetDescribe_Implementation() const
{
	return FText();
}

void UXD_GameEventElementBase::FinishGameEventElement(class UXD_GameEventGraphEdge* NextEdge)
{
	if (bIsFinish == false)
	{
		bIsFinish = true;
		GameEventOuter_GameEventSequence->InvokeFinishGameEventSequence(this, NextEdge);
		GameEventSystem_Display_LOG("%s完成[%s]中的游戏事件序列[%s]中的游戏事件元素[%s]", *UXD_DebugFunctionLibrary::GetDebugName(GetGameEventOwnerCharacter()), *GetGameEvent()->GetGameEventName().ToString(), *GameEventOuter_GameEventSequence->GetDescribe().ToString(), *GetName());
	}
}

void UXD_GameEventElementBase::IsFinishFalse()
{
	if (bIsFinish == true)
	{
		bIsFinish = false;
		if (bIsMust && GameEventOuter_GameEventSequence->GameEventElementList.Contains(this))
		{
			GameEventOuter_GameEventSequence->WhenGameEventElementReUnfinished();
		}
	}
}

void UXD_GameEventElementBase::ActivateGameEventElement()
{
	WhenActivateGameEventElement(GetGameEventOwnerCharacter(), GetGameEventOwnerController());
}

void UXD_GameEventElementBase::UnactiveGameEventElement()
{
	WhenUnactiveGameEventElement(GetGameEventOwnerCharacter(), GetGameEventOwnerController());
	if (bIsFinish)
	{
		WhenFinishGameEventElement(GetGameEventOwnerCharacter(), GetGameEventOwnerController());
	}
}

void UXD_GameEventElementBase::SetTemplate_Implementation(const class UGenericGraphNodeBase* NodeBase)
{

}

TArray<FGenericGraph_NewEdgeParamers> UXD_GameEventElementBase::GetFinishGameEventElementBranchEdgeTemplates_Implementation(UObject* Outer, const class UXD_GameEventGraphNode* GameEventGraphNode) const
{
	return {};
}

class AController* UXD_GameEventElementBase::GetGameEventOwnerController() const
{
	return GameEventOuter_GameEventSequence->GameEventOuter_GameEventBase->GeGameEventOwnerController();
}

class APawn* UXD_GameEventElementBase::GetGameEventOwnerCharacter() const
{
	return GameEventOuter_GameEventSequence->GameEventOuter_GameEventBase->GetGameEventOwnerCharacter();
}

class UXD_GameEventBase* UXD_GameEventElementBase::GetGameEvent() const
{
	return GameEventOuter_GameEventSequence ? GameEventOuter_GameEventSequence->GameEventOuter_GameEventBase : nullptr;
}

#undef LOCTEXT_NAMESPACE

