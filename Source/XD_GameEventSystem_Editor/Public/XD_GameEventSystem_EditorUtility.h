// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "XD_GameEventSystemUtility.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(XD_GameEventSystem_Editor_Log, Log, All);

#define GameEventSystem_Editor_Display_LOG(FMT, ...) UE_LOG(XD_GameEventSystem_Editor_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define GameEventSystem_Editor_Warning_LOG(FMT, ...) UE_LOG(XD_GameEventSystem_Editor_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define GameEventSystem_Editor_Error_Log(FMT, ...) UE_LOG(XD_GameEventSystem_Editor_Log, Error, TEXT(FMT), ##__VA_ARGS__)