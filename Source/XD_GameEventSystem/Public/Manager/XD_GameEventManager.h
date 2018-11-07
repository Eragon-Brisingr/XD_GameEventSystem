// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XD_SaveGameInterface.h"
#include "XD_GameEventManager.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XD_GAMEEVENTSYSTEM_API UXD_GameEventManager : public UActorComponent, public IXD_SaveGameInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXD_GameEventManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override; 

	virtual void WhenPostLoad_Implementation() override;
private:
	void ReplicatedGameEventList(const TArray<UXD_GameEventBase*>& GameEventList, bool& WroteSomething, class UActorChannel * Channel, class FOutBunch * Bunch, FReplicationFlags * RepFlags);
	//游戏事件
public:
	UPROPERTY()
	TArray<class UXD_GameEventBase*> PreUnderwayGameEventList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "游戏事件", ReplicatedUsing = OnRep_UnderwayGameEventList, SaveGame)
	TArray<class UXD_GameEventBase*> UnderwayGameEventList;
	UFUNCTION()
	void OnRep_UnderwayGameEventList();

	UPROPERTY()
	TArray<class UXD_GameEventBase*> PreFinishGameEventList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "游戏事件", ReplicatedUsing = OnRep_FinishGameEventList, SaveGame)
	TArray<class UXD_GameEventBase*> FinishGameEventList;
	UFUNCTION()
	void OnRep_FinishGameEventList();

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件", BlueprintAuthorityOnly)
	void ApplyGameEvent(class UXD_GameEventGraph* GameEventGraph);

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	bool IsGameEventExistInUnderwayList(class UXD_GameEventGraph* GameEvent);

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	bool IsGameEventExistInFinishList(class UXD_GameEventGraph* GameEvent);

	UFUNCTION(BlueprintCallable, Category = "角色|游戏事件")
	bool IsGameEventExist(class UXD_GameEventGraph* GameEvent);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAcceptGameEvent, class UXD_GameEventBase*, GameEvent);
	UPROPERTY(BlueprintAssignable, Category = "角色|游戏事件")
	FOnAcceptGameEvent OnAcceptGameEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveUnderwayGameEvent, class UXD_GameEventBase*, GameEvent);
	UPROPERTY(BlueprintAssignable, Category = "角色|游戏事件")
	FOnRemoveUnderwayGameEvent OnRemoveUnderwayGameEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinishGameEvent, class UXD_GameEventBase*, GameEvent);
	UPROPERTY(BlueprintAssignable, Category = "角色|游戏事件")
	FOnFinishGameEvent OnFinishGameEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveFinishGameEvent, class UXD_GameEventBase*, GameEvent);
	UPROPERTY(BlueprintAssignable, Category = "角色|游戏事件")
	FOnRemoveFinishGameEvent OnRemoveFinishGameEvent;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFinishedGameEventSequence, class UXD_GameEventBase*, GameEvent, class UXD_GameEventSequenceBase*, FinishedGameEventSequence, class UXD_GameEventSequenceBase*, UnderwayGameEventSequences);
	UPROPERTY(BlueprintAssignable, Category = "角色|游戏事件")
	FOnFinishedGameEventSequence OnFinishedGameEventSequence;

};
