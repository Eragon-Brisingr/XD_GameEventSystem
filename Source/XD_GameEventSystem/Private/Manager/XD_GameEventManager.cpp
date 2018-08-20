// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventManager.h"

// Sets default values for this component's properties
UXD_GameEventManager::UXD_GameEventManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UXD_GameEventManager::BeginPlay()
{
	Super::BeginPlay();

	// ...

	SetIsReplicated(true);
}


// Called every frame
void UXD_GameEventManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UXD_GameEventManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UXD_GameEventManager, UnderwayGameEventList);
	DOREPLIFETIME(UXD_GameEventManager, FinishGameEventList);
}

bool UXD_GameEventManager::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	ReplicatedGameEventList(UnderwayGameEventList, WroteSomething, Channel, Bunch, RepFlags);
	ReplicatedGameEventList(FinishGameEventList, WroteSomething, Channel, Bunch, RepFlags);

	return WroteSomething;
}

void UXD_GameEventManager::WhenLoad_Implementation()
{
	//恢复游戏事件
	for (UXD_GameEventBase* GameEvent : UnderwayGameEventList)
	{
		GameEvent->ReinitGameEvent(this);

		GetWorld()->GetTimerManager().SetTimerForNextTick([=]()
		{
			GameEvent->ReactiveGameEvent();
		});
	}
	for (UXD_GameEventBase* GameEvent : FinishGameEventList)
	{
		GameEvent->ReinitGameEvent(this);
	}
}

void UXD_GameEventManager::ReplicatedGameEventList(const TArray<UXD_GameEventBase*>& GameEventList, bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags)
{
	for (UXD_GameEventBase* GameEvent : GameEventList)
	{
		if (GameEvent)
		{
			WroteSomething |= Channel->ReplicateSubobject(GameEvent, *Bunch, *RepFlags);
			GameEvent->ReplicatedGameEventSequence(WroteSomething, Channel, Bunch, RepFlags);
		}
	}
}

void UXD_GameEventManager::OnRep_UnderwayGameEventList()
{
	for (auto AddGameEvent : TSet<UXD_GameEventBase*>(UnderwayGameEventList).Difference(TSet<UXD_GameEventBase*>(PreUnderwayGameEventList)))
	{
		if (AddGameEvent)
		{
			AddGameEvent->GameEventOwner = this;
			OnAcceptGameEvent.Broadcast(AddGameEvent);
		}
	}

	//提示游戏事件完成
	{
		for (auto RemoveGameEvent : TSet<UXD_GameEventBase*>(PreUnderwayGameEventList).Difference(TSet<UXD_GameEventBase*>(UnderwayGameEventList)))
		{
			if (RemoveGameEvent)
			{
				RemoveGameEvent->WhenFinishGameEvent();
				OnRemoveUnderwayGameEvent.Broadcast(RemoveGameEvent);
			}
		}
	}
	PreUnderwayGameEventList = UnderwayGameEventList;
}

void UXD_GameEventManager::OnRep_FinishGameEventList()
{
	if (OnFinishGameEvent.IsBound())
	{
		for (auto FinishGameEvent : TSet<UXD_GameEventBase*>(FinishGameEventList).Difference(TSet<UXD_GameEventBase*>(PreFinishGameEventList)))
		{
			if (FinishGameEvent)
			{
				FinishGameEvent->GameEventOwner = this;
				OnFinishGameEvent.Broadcast(FinishGameEvent);
			}
		}
	}
	if (OnRemoveFinishGameEvent.IsBound())
	{
		for (auto RemoveGameEvent : TSet<UXD_GameEventBase*>(PreFinishGameEventList).Difference(TSet<UXD_GameEventBase*>(FinishGameEventList)))
		{
			if (RemoveGameEvent)
			{
				OnRemoveFinishGameEvent.Broadcast(RemoveGameEvent);
			}
		}
	}
	PreFinishGameEventList = FinishGameEventList;
}

void UXD_GameEventManager::ApplyGameEvent(class UXD_GameEventGraph* GameEventGraph)
{
	if (GetOwner()->HasAuthority() && GameEventGraph)
	{
		class UXD_GameEventBase* GameEvent = UXD_GameEventBase::NewGameEvent(this, GameEventGraph);
		GameEventSystem_Display_LOG("%s 接受了游戏事件 %s", *UXD_DebugFunctionLibrary::GetDebugName(GetOwner()), *GameEvent->GetGameEventName().ToString());

		UnderwayGameEventList.Add(GameEvent);
		OnRep_UnderwayGameEventList();
		//最后再激活，防止游戏事件瞬间完成
		GameEvent->ActiveGameEvent(this);
	}
}

bool UXD_GameEventManager::IsGameEventExistInUnderwayList(class UXD_GameEventGraph* GameEvent)
{
	if (GameEvent)
	{
		for (UXD_GameEventBase* E_GameEvent : UnderwayGameEventList)
		{
			if (E_GameEvent->IsEqualWithOtherGameEvent(GameEvent))
			{
				return true;
			}
		}
	}
	return false;
}

bool UXD_GameEventManager::IsGameEventExistInFinishList(class UXD_GameEventGraph* GameEvent)
{
	if (GameEvent)
	{
		for (UXD_GameEventBase* E_GameEvent : FinishGameEventList)
		{
			if (E_GameEvent->IsEqualWithOtherGameEvent(GameEvent))
			{
				return true;
			}
		}
	}
	return false;
}

bool UXD_GameEventManager::IsGameEventExist(class UXD_GameEventGraph* GameEvent)
{
	return IsGameEventExistInUnderwayList(GameEvent) || IsGameEventExistInFinishList(GameEvent);
}


