// Fill out your copyright notice in the Description page of Project Settings.

#include "XD_GameEventSequenceBase.h"
#include "XD_GameEventBase.h"
#include "Graph/XD_GameEventGraph_Impl.h"


#define LOCTEXT_NAMESPACE "游戏事件"

class UWorld* UXD_GameEventSequenceBase::GetWorld() const
{
	return OwingGameEvent ? OwingGameEvent->GetWorld() : nullptr;
}

bool UXD_GameEventSequenceBase::IsSupportedForNetworking() const
{
	return true;
}

void UXD_GameEventSequenceBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}

	DOREPLIFETIME(UXD_GameEventSequenceBase, GameEventElementList);
}

void UXD_GameEventSequenceBase::ReplicatedGameEventElement(bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags)
{
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		if (GameEventElement)
		{
			WroteSomething |= Channel->ReplicateSubobject(GameEventElement, *Bunch, *RepFlags);
		}
	}
}

void UXD_GameEventSequenceBase::ActiveGameEventSequence()
{
	GameEventSystem_Display_LOG("%s激活[%s]中的游戏事件序列[%s]", *UXD_DebugFunctionLibrary::GetDebugName(GetGameEventOwnerCharacter()), *OwingGameEvent->GetGameEventName().ToString(), *GetDescribe().ToString());
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		GameEventElement->ActivateGameEventElement();
	}
}

bool UXD_GameEventSequenceBase::HasMustGameEventElement()
{
	for (auto GameEventElement : GameEventElementList)
	{
		if (GameEventElement->bIsMust)
		{
			return true;
		}
	}
	return false;
}

void UXD_GameEventSequenceBase::DeactiveGameEventSequence()
{
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		GameEventElement->UnactiveGameEventElement();
	}
}

void UXD_GameEventSequenceBase::InvokeFinishGameEventSequence(class UXD_GameEventElementBase* GameEventElement, class UXD_GameEventGraphEdge* NextEdge)
{
	unimplemented();
}

bool UXD_GameEventSequenceBase::IsEveryMustGameEventElementFinished() const
{
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		if (GameEventElement->bIsMust && !GameEventElement->IsFinished())
		{
			return false;
		}
	}
	return true;
}

void UXD_GameEventSequenceBase::DrawHintInWorld(class AHUD* ARPG_HUD, int32 Index, bool IsFinishBranch)
{
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		GameEventElement->DrawHintInWorld(ARPG_HUD, Index, false);
	}
}

void UXD_GameEventSequenceBase::AddGameEventElement(class UXD_GameEventElementBase* GameEventElement)
{
	GameEventElementList.Add(GameEventElement);
}

void UXD_GameEventSequenceBase::ReinitGameEventSequence()
{
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		if (GameEventElement)
		{
			GameEventElement->OwingGameEventSequence = this;
		}
	}
}

FText UXD_GameEventSequenceBase::GetDescribe() const
{
	return GameEventSequenceTemplate.IsValid() ? GameEventSequenceTemplate->GetDescribe() : LOCTEXT("没找到游戏事件序列对应的模板", "没找到游戏事件序列对应的模板");
}

void UXD_GameEventSequenceBase::OnRep_GameEventElementList()
{
	for (UXD_GameEventElementBase* GameEventElement : GameEventElementList)
	{
		if (GameEventElement)
		{
			GameEventElement->OwingGameEventSequence = this;
		}
	}
}

class APawn* UXD_GameEventSequenceBase::GetGameEventOwnerCharacter() const
{
	return OwingGameEvent->GetGameEventOwnerCharacter();
}

void UGameEventSequence_Branch::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGameEventSequence_Branch, GameEventElementFinishList);

}

void UGameEventSequence_Branch::ReplicatedGameEventElement(bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags)
{
	Super::ReplicatedGameEventElement(WroteSomething, Channel, Bunch, RepFlags);
	for (const FGameEventElementFinishWarpper& GameEventElementFinishWarpper : GameEventElementFinishList)
	{
		if (GameEventElementFinishWarpper.GameEventElement)
		{
			WroteSomething |= Channel->ReplicateSubobject(GameEventElementFinishWarpper.GameEventElement, *Bunch, *RepFlags);
		}
	}
}

void UGameEventSequence_Branch::ReinitGameEventSequence()
{
	Super::ReinitGameEventSequence();
	for (FGameEventElementFinishWarpper& GameEventElementFinishWarpper : GameEventElementFinishList)
	{
		if (GameEventElementFinishWarpper.GameEventElement)
		{
			GameEventElementFinishWarpper.GameEventElement->OwingGameEventSequence = this;
		}
	}
}

void UGameEventSequence_Branch::ActiveGameEventSequence()
{
	Super::ActiveGameEventSequence();
	InvokeActiveFinishList();
}

void UGameEventSequence_Branch::DeactiveGameEventSequence()
{
	Super::DeactiveGameEventSequence();
	DeactiveFinishBranchs();
}

void UGameEventSequence_Branch::InvokeFinishGameEventSequence(class UXD_GameEventElementBase* GameEventElement, class UXD_GameEventGraphEdge* NextEdge)
{
	//激活结束游戏事件的关键游戏事件
	if (GameEventElementList.Contains(GameEventElement))
	{
		InvokeActiveFinishList();
	}
	//结束游戏事件
	else
	{
		UXD_GameEventSequenceBase* FinishGameEventSequence = OwingGameEvent->GetUnderwayGameEventSequence();
		for (FGameEventElementFinishWarpper& GameEventElementFinishWarpper : GameEventElementFinishList)
		{
			if (GameEventElement == GameEventElementFinishWarpper.GameEventElement)
			{
				if (GameEventElementFinishWarpper.GameEventFinishBranch.IsValid())
				{
					if (GameEventElementFinishWarpper.GameEventFinishBranch->ChildrenNodes.Num() > 0)
					{
						OwingGameEvent->SetAndActiveNextGameEventSequence(GameEventElementFinishWarpper.GameEventFinishBranch->GetGameEventSequence(OwingGameEvent, NextEdge));
					}
					else
					{
						OwingGameEvent->SetAndActiveNextGameEventSequence(nullptr);
					}
				}
				break;
			}
		}
	}
}

void UGameEventSequence_Branch::WhenGameEventElementReactive()
{
	DeactiveFinishBranchs();
}

void UGameEventSequence_Branch::DrawHintInWorld(class AHUD* ARPG_HUD, int32 Index, bool IsFinishBranch)
{
	Super::DrawHintInWorld(ARPG_HUD, Index, IsFinishBranch);
	if (bIsFinishListActive)
	{
		for (FGameEventElementFinishWarpper& GameEventElementFinishWarpper : GameEventElementFinishList)
		{
			GameEventElementFinishWarpper.GameEventElement->DrawHintInWorld(ARPG_HUD, Index, true);
		}
	}
}

void UGameEventSequence_Branch::InvokeActiveFinishList()
{
	if (IsEveryMustGameEventElementFinished())
	{
		if (GameEventElementFinishList.Num() == 0)
		{
			OwingGameEvent->FinishGameEvent();
		}
		else
		{
			if (bIsFinishListActive == false)
			{
				for (FGameEventElementFinishWarpper& GameEventElementFinish : GameEventElementFinishList)
				{
					GameEventElementFinish.GameEventElement->ActivateGameEventElement();
				}
				bIsFinishListActive = true;
			}
		}
	}
}

void UGameEventSequence_Branch::DeactiveFinishBranchs()
{
	bIsFinishListActive = false;
	for (FGameEventElementFinishWarpper& GameEventElementFinishWarpper : GameEventElementFinishList)
	{
		GameEventElementFinishWarpper.GameEventElement->UnactiveGameEventElement();
	}
}

void UGameEventSequence_Branch::OnRep_GameEventElementFinishList()
{
	for (FGameEventElementFinishWarpper& GameEventElementFinishWarpper : GameEventElementFinishList)
	{
		if (GameEventElementFinishWarpper.GameEventElement)
		{
			GameEventElementFinishWarpper.GameEventElement->OwingGameEventSequence = this;
		}
	}
}

void UGameEventSequence_List::InvokeFinishGameEventSequence(class UXD_GameEventElementBase* GameEventElement, class UXD_GameEventGraphEdge* NextEdge)
{
	if (IsEveryMustGameEventElementFinished())
	{
		if (NextGameEvent.IsValid())
		{
			OwingGameEvent->SetAndActiveNextGameEventSequence(NextGameEvent->GetGameEventSequence(OwingGameEvent, NextEdge));
		}
		else
		{
			OwingGameEvent->SetAndActiveNextGameEventSequence(nullptr);
		}
	}
}

#undef LOCTEXT_NAMESPACE
