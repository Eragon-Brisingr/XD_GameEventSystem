// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "XD_GameEventSystemUtility.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(XD_GameEventSystem_Log, Log, All);

#define GameEventSystem_Display_LOG(FMT, ...) UE_LOG(XD_GameEventSystem_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define GameEventSystem_Warning_LOG(FMT, ...) UE_LOG(XD_GameEventSystem_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define GameEventSystem_Error_Log(FMT, ...) UE_LOG(XD_GameEventSystem_Log, Error, TEXT(FMT), ##__VA_ARGS__)