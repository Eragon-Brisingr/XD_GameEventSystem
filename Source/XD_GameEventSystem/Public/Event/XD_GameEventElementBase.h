// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <UserWidget.h>
#include "XD_GameEventElementBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, collapseCategories)
class XD_GAMEEVENTSYSTEM_API UXD_GameEventElementBase : public UObject
{
	GENERATED_BODY()

public:
	UXD_GameEventElementBase();

	virtual class UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking()const override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const;

private:
	UPROPERTY(SaveGame, Replicated)
	uint8 bIsFinished : 1;
public:
	//为真则为必要游戏事件
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "游戏事件", Meta = (DisplayName = "必要游戏事件", ExposeOnSpawn = true), Replicated)
	uint8 bIsMust : 1;

	//为真则显示于玩家面板上
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly, Category = "游戏事件", Meta = (DisplayName = "显示于玩家面板", ExposeOnSpawn = true), Replicated)
	uint8 bIsShowGameEventElement : 1;

public:
	//获取游戏事件元素描述
	UFUNCTION(BlueprintPure, Category = "角色|游戏事件")
	FText GetDescribe();

	UFUNCTION(BlueprintNativeEvent, Category="角色|游戏事件", meta = (DisplayName = "GetDescribe"))
	FText ReceiveGetDescribe() const;
	virtual FText ReceiveGetDescribe_Implementation() const;

	//显示游戏事件提示在指南针上
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "角色|游戏事件")
	void DrawHintOnHUD(const FPaintContext& PaintContext, const FGeometry& Geometry, APlayerController* Controller, int32 Index, bool IsFinishBranch);
	virtual void DrawHintOnHUD_Implementation(const FPaintContext& PaintContext, const FGeometry& Geometry, APlayerController* Controller, int32 Index, bool IsFinishBranch) {};

	//显示游戏事件提示在世界中
	UFUNCTION(BlueprintNativeEvent, Category = "角色|游戏事件")
	void DrawHintInWorld(class AHUD* ARPG_HUD, int32 Index, bool IsFinishBranch);
	virtual void DrawHintInWorld_Implementation(class AHUD* ARPG_HUD, int32 Index, bool IsFinishBranch){}

	//游戏事件是否完成
	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	FORCEINLINE bool IsFinished() const { return bIsFinished; }

	/**
	* 完成该任务序列
	* @param	NextEdge		游戏事件元素中可能也存在分支，比如说和某人对话中出现的分支，用Edge区分
	*/
	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件", meta = (AdvancedDisplay = "0"))
	void FinishGameEventElement(class UXD_GameEventGraphEdge* NextEdge);

	//该游戏元素又未完成了调用这个 e.g.目标数量道具减少
	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	void SetReactive();

public:
	void ActivateGameEventElement();
	//用于激活该游戏事件元素的检查事件
	UFUNCTION(BlueprintAuthorityOnly, BlueprintNativeEvent, Category = "角色|游戏事件")
	void WhenActivateGameEventElement(class APawn* GameEventOwnerCharacter, class AController* GameEventOwner);
	virtual void WhenActivateGameEventElement_Implementation(class APawn* GameEventOwnerCharacter, class AController* GameEventOwner){}

	void UnactiveGameEventElement();
	//用于反激活该游戏事件元素的检查事件
	UFUNCTION(BlueprintAuthorityOnly, BlueprintNativeEvent, Category = "角色|游戏事件")
	void WhenUnactiveGameEventElement(class APawn* GameEventOwnerCharacter, class AController* GameEventOwner);
	virtual void WhenUnactiveGameEventElement_Implementation(class APawn* GameEventOwnerCharacter, class AController* GameEventOwner){}

	//游戏事件完成后调用
	UFUNCTION(BlueprintAuthorityOnly, BlueprintNativeEvent, Category = "角色|游戏事件")
	void WhenFinishGameEventElement(class APawn* GameEventOwnerCharacter, class AController* GameEventOwner);
	virtual void WhenFinishGameEventElement_Implementation(class APawn* GameEventOwnerCharacter, class AController* GameEventOwner){}

	//假如该游戏事件元素能产生不同分支需重载下面几个函数
#if WITH_EDITORONLY_DATA
	bool bHasMultiBranch;
#endif
	//设置运行时中的游戏事件元素对于与图表的游戏事件元素
	UFUNCTION(BlueprintNativeEvent, Category = "角色|游戏事件")
	void SetTemplate(const class UGenericGraphNodeBase* inTemplate);
	void SetTemplate_Implementation(const class UGenericGraphNodeBase* inTemplate){}

	//游戏事件元素产生的分支，给编辑器获取分支节点模板用
	UFUNCTION(BlueprintNativeEvent, Category = "角色|游戏事件")
	TArray<FGenericGraph_NewEdgeParamers> GetFinishGameEventElementBranchEdgeTemplates(UObject* Outer, const class UXD_GameEventGraphNode* GameEventGraphNode) const;
	virtual TArray<FGenericGraph_NewEdgeParamers> GetFinishGameEventElementBranchEdgeTemplates_Implementation(UObject* Outer, const class UXD_GameEventGraphNode* GameEventGraphNode) const;

	//双击该节点编辑器跳转的目标
	UFUNCTION(BlueprintNativeEvent, Category = "泛用图表")
	UObject* GetJumpTargetForDoubleClick() const;
	virtual UObject* GetJumpTargetForDoubleClick_Implementation() const { return nullptr; }
public:
	UPROPERTY(BlueprintReadOnly, Category = "角色|游戏事件")
	class UXD_GameEventSequenceBase* OwingGameEventSequence;

	UFUNCTION(BlueprintPure, Category = "角色|游戏事件")
	class AController* GetGameEventOwnerController() const;
	UFUNCTION(BlueprintPure, Category = "角色|游戏事件")
	class APawn* GetGameEventOwnerCharacter() const;
	UFUNCTION(BlueprintPure, Category = "角色|游戏事件")
	class UXD_GameEventBase* GetGameEvent() const;

};

