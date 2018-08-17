// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "XD_GameEventSequenceBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventSequenceBase : public UObject
{
	GENERATED_BODY()
public:
	virtual class UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking()const override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const;

	virtual void ReplicatedGameEventElement(bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags);
public:
	void AddGameEventElement(class UXD_GameEventElementBase* GameEventElement);

	virtual void ReinitGameEventSequence();
public:
	UPROPERTY(BlueprintReadOnly, Category = "角色|游戏事件", SaveGame)
	TSoftObjectPtr<class UXD_GameEventGraphNode_GameEventSequenceBase> GameEventSequenceTemplate;

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	FText GetDescribe() const;

	UPROPERTY(BlueprintReadOnly, Category = "角色|游戏事件")
	uint8 bIsFinishListActive : 1;

	UPROPERTY(BlueprintReadOnly, Category = "角色|游戏事件", ReplicatedUsing = OnRep_GameEventElementList, SaveGame)
	TArray<class UXD_GameEventElementBase*> GameEventElementList;
	UFUNCTION()
	void OnRep_GameEventElementList();

	UPROPERTY(BlueprintReadOnly, Category = "角色|游戏事件")
	class UXD_GameEventBase* GameEventOuter_GameEventBase;

	virtual void ActiveGameEventSequence();

	bool HasMustGameEventElement();

	virtual void FinishAllGameEventElement();

	virtual void InvokeFinishGameEventSequence(class UXD_GameEventElementBase* GameEventElement, class UXD_GameEventGraphEdge* NextEdge);

	virtual void WhenGameEventElementReUnfinished(){}

	bool IsEveryMustGameEventElementFinished() const;

	virtual void DrawHintInWorld(class AHUD* ARPG_HUD, int32 Index, bool IsFinishBranch);
public:
	UFUNCTION(BlueprintPure, Category = "角色|游戏事件")
	class APawn* GetGameEventOwnerCharacter() const;
	 
};

//TODO 完成必须的游戏事件元素之后出现分支
UCLASS()
class XD_GAMEEVENTSYSTEM_API UGameEventSequence_Branch : public UXD_GameEventSequenceBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const;

	virtual void ReplicatedGameEventElement(bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags);

	virtual void ReinitGameEventSequence() override;

	virtual void ActiveGameEventSequence() override;

	virtual void InvokeFinishGameEventSequence(class UXD_GameEventElementBase* GameEventElement, class UXD_GameEventGraphEdge* NextEdge);

	virtual void WhenGameEventElementReUnfinished();

	virtual void DrawHintInWorld(class AHUD* ARPG_HUD, int32 Index, bool IsFinishBranch);

	void InvokeActiveFinishList();

	void DeactiveFinishBranchs();
public:
	UPROPERTY(BlueprintReadWrite, Category = "角色|游戏事件", ReplicatedUsing = OnRep_GameEventElementFinishList, SaveGame)
	TArray<FGameEventElementFinishWarpper> GameEventElementFinishList;
	UFUNCTION()
	void OnRep_GameEventElementFinishList();

};

//TODO 完成所有必须的游戏事件元素之后即进行下一个序列
UCLASS()
class XD_GAMEEVENTSYSTEM_API UGameEventSequence_FinishAllElement : public UXD_GameEventSequenceBase
{
	GENERATED_BODY()
public:
};

