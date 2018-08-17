// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventBase.h"
#include "Manager/XD_GameEventManager.h"
#include "Event/XD_GameEventSequenceBase.h"
#include "Graph/XD_GameEventGraph.h"
#include <UnrealNetwork.h>
#include <Engine/ActorChannel.h>
#include "XD_DebugFunctionLibrary.h"
#include "XD_GameEventSystemUtility.h"

FGameEventElementFinishWarpper::FGameEventElementFinishWarpper(class UXD_GameEventElementBase* GameEventElement, const TSoftObjectPtr<class UXD_GameEventGraphNode>& GameEventFinishBranch)
	: GameEventElement(GameEventElement), GameEventFinishBranch(GameEventFinishBranch)
{

}

FGameEventElementFinishWarpper::FGameEventElementFinishWarpper(class UXD_GameEventElementBase* GameEventElement)
	: GameEventElement(GameEventElement)
{

}

UXD_GameEventBase::UXD_GameEventBase()
	:bIsShowGameEvent(true)
{

}

UXD_GameEventBase* UXD_GameEventBase::NewGameEvent(UObject* WorldContextObject, class UXD_GameEventGraph* GameEventGraph)
{
	UXD_GameEventBase* GameEvent = NewObject<UXD_GameEventBase>(WorldContextObject);
	GameEvent->WorldContext = WorldContextObject->GetWorld()->GetGameInstance()->GetWorldContext();
	GameEvent->GameEventTemplate = GameEventGraph;
	return GameEvent;
}

void UXD_GameEventBase::ReinitGameEvent(class UXD_GameEventManager* GameEventOwner)
{
	for (UXD_GameEventSequenceBase* GameEventSequence : CurrentGameEventSequenceList)
	{
		this->GameEventOwner = GameEventOwner;
		WorldContext = GameEventOwner->GetWorld()->GetGameInstance()->GetWorldContext();
		if (GameEventSequence)
		{
			GameEventSequence->GameEventOuter_GameEventBase = this;
			GameEventSequence->ReinitGameEventSequence();
		}
	}
}

void UXD_GameEventBase::ReactiveGameEvent()
{
	GetUnderwayGameEventSequence()->ActiveGameEventSequence();
}

const class UXD_GameEventGraph* UXD_GameEventBase::GetGameEventTemplate() const
{
	return GameEventTemplate;
}

FText UXD_GameEventBase::GetGameEventName() const
{
	return GameEventTemplate->GameEventName;
}

bool UXD_GameEventBase::IsEqualWithOtherGameEvent(class UXD_GameEventGraph* GameEvent) const
{
	return GameEventTemplate == GameEvent;
}

#if WITH_EDITOR
void UXD_GameEventBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif //WITH_EDITOR

class UWorld* UXD_GameEventBase::GetWorld() const
{
	return GameEventOwner ? GameEventOwner->GetWorld() : WorldContext ? WorldContext->World() : nullptr;
}

bool UXD_GameEventBase::IsSupportedForNetworking() const
{
	return bIsShowGameEvent;
}

void UXD_GameEventBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}

	DOREPLIFETIME(UXD_GameEventBase, CurrentGameEventSequenceList);
	DOREPLIFETIME(UXD_GameEventBase, GameEventTemplate);
}

void UXD_GameEventBase::ReplicatedGameEventSequence(bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags)
{
	for (UXD_GameEventSequenceBase* GameEventSequence : CurrentGameEventSequenceList)
	{
		if (GameEventSequence)
		{
			WroteSomething |= Channel->ReplicateSubobject(GameEventSequence, *Bunch, *RepFlags);
			GameEventSequence->ReplicatedGameEventElement(WroteSomething, Channel, Bunch, RepFlags);
		}
	}
}

void UXD_GameEventBase::OnRep_CurrentGameEventSequenceList()
{
	for (UXD_GameEventSequenceBase* GameEventSequence : CurrentGameEventSequenceList)
	{
		if (GameEventSequence)
		{
			GameEventSequence->GameEventOuter_GameEventBase = this;
		}
	}
	if (CurrentGameEventSequenceList.Num() > 1)
	{
		WhenFinishedGameEventSequence(CurrentGameEventSequenceList[CurrentGameEventSequenceList.Num() - 2], GetUnderwayGameEventSequence());
	}
}

class APawn* UXD_GameEventBase::GetGameEventOwnerCharacter() const
{
	return GeGameEventOwnerController()->GetPawn();
}

class AController* UXD_GameEventBase::GeGameEventOwnerController() const
{
	return Cast<AController>(GameEventOwner->GetOwner());
}

void UXD_GameEventBase::ActiveGameEvent(class UXD_GameEventManager* GameEventExecuter)
{
	GameEventOwner = GameEventExecuter;
	GameEventState = EGameEventState::Underway;

	CurrentGameEventSequenceList.Add(GameEventTemplate->FirstGameEventGraphNode->GetGameEventSequence(this, nullptr));

	CurrentGameEventSequenceList[0]->ActiveGameEventSequence();
}

void UXD_GameEventBase::FinishGameEvent()
{
	GameEventState = EGameEventState::Finish_Succeed;
	GameEventOwner->UnderwayGameEventList.Remove(this);
	GameEventOwner->FinishGameEventList.Add(this);
	GameEventOwner->OnRep_UnderwayGameEventList();
	GameEventOwner->OnRep_FinishGameEventList();

	GameEventSystem_Display_LOG("%s完成游戏事件[%s]", *UXD_DebugFunctionLibrary::GetDebugName(GetGameEventOwnerCharacter()), *GameEventTemplate->GetName());
}

void UXD_GameEventBase::WhenFinishedGameEventSequence_Implementation(class UXD_GameEventSequenceBase* FinishedGameEventSequence, class UXD_GameEventSequenceBase* UnderwayGameEventSequences)
{
	GameEventOwner->OnFinishedGameEventSequence.Broadcast(this, FinishedGameEventSequence, UnderwayGameEventSequences);
}

void UXD_GameEventBase::ForceFinishGameEvent(EGameEventState State)
{
	GetUnderwayGameEventSequence()->FinishAllGameEventElement();
	FinishGameEvent();
	GameEventState = State;
}
