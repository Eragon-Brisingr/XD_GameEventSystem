// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "XD_GameEventBase.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EGameEventState : uint8
{
	Underway UMETA(DisplayName = "进行中"),
	Finish_Succeed UMETA(DisplayName = "成功"),
	Finish_Failed UMETA(DisplayName = "失败")
};

USTRUCT(BlueprintType)
struct XD_GAMEEVENTSYSTEM_API FGameEventElementFinishWarpper
{
	GENERATED_USTRUCT_BODY()

public:
	FGameEventElementFinishWarpper() = default;

	FGameEventElementFinishWarpper(class UXD_GameEventElementBase* GameEventElement);

	FGameEventElementFinishWarpper(class UXD_GameEventElementBase* GameEventElement, const TSoftObjectPtr<class UXD_GameEventGraphNode>& GameEventFinishBranch);

public:
	UPROPERTY(BlueprintReadWrite, Category = "游戏事件", SaveGame)
	class UXD_GameEventElementBase* GameEventElement;

	UPROPERTY(SaveGame)
	TSoftObjectPtr<class UXD_GameEventGraphNode> GameEventFinishBranch;
};


UCLASS(Blueprintable, BlueprintType, hidedropdown)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventBase : public UObject
{
	GENERATED_BODY()

public:
	UXD_GameEventBase();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext), Category = "角色|游戏事件")
	static UXD_GameEventBase* NewGameEvent(UObject* WorldContextObject, class UXD_GameEventGraph* GameEventGraph);

	void ReinitGameEvent(class UXD_GameEventManager* GameEventOwner);

	void ReactiveGameEvent();
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR
	struct FWorldContext* WorldContext;

	virtual class UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const;

	virtual void ReplicatedGameEventSequence(bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags);
public:
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "游戏事件", Replicated)
	EGameEventState GameEventState;
	
	UPROPERTY(SaveGame, Replicated)
	const class UXD_GameEventGraph* GameEventTemplate;

	//为真则显示于玩家面板上
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "游戏事件", Meta = (ExposeOnSpawn = true), Replicated)
	uint8 bIsShowGameEvent : 1;

public:
	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	bool IsFinished() const { return GameEventState != EGameEventState::Underway; }

	const class UXD_GameEventGraph* GetGameEventTemplate() const;

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	FText GetGameEventName() const;

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	bool IsEqualWithOtherGameEvent(class UXD_GameEventGraph* GameEvent) const;
public:
	UPROPERTY(BlueprintReadOnly, Category = "游戏事件", ReplicatedUsing = OnRep_CurrentGameEventSequenceList, SaveGame)
	TArray<UXD_GameEventSequenceBase*> CurrentGameEventSequenceList;
	UFUNCTION()
	void OnRep_CurrentGameEventSequenceList();

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	FORCEINLINE UXD_GameEventSequenceBase* GetUnderwayGameEventSequence() const
	{
		return CurrentGameEventSequenceList[CurrentGameEventSequenceList.Num() - 1];
	}

	//NextGameEventSequence == nullptr 则完成该任务
	void SetAndActiveNextGameEventSequence(class UXD_GameEventSequenceBase* NextGameEventSequence);

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	class APawn* GetGameEventOwnerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	class AController* GeGameEventOwnerController() const;

	UPROPERTY(BlueprintReadOnly, Category = "角色|游戏事件")
	class UXD_GameEventManager* GameEventOwner;
public:
	void ActiveGameEvent(class UXD_GameEventManager* GameEventExecuter);

	void FinishGameEvent();
	
	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	void ForceFinishGameEvent(EGameEventState State = EGameEventState::Finish_Failed);

	UFUNCTION(BlueprintNativeEvent, Category = "游戏事件")
	void WhenFinishGameEvent();
	virtual void WhenFinishGameEvent_Implementation(){}

	UFUNCTION(BlueprintNativeEvent, Category = "游戏事件")
	void WhenFinishedGameEventSequence(class UXD_GameEventSequenceBase* FinishedGameEventSequence, class UXD_GameEventSequenceBase* UnderwayGameEventSequences);
	virtual void WhenFinishedGameEventSequence_Implementation(class UXD_GameEventSequenceBase* FinishedGameEventSequence, class UXD_GameEventSequenceBase* UnderwayGameEventSequences);

	UFUNCTION(BlueprintNativeEvent, Category = "游戏事件")
	void InitGameEvent();
	void InitGameEvent_Implementation(){}
};
