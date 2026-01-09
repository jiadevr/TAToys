// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NotifyHelper.generated.h"

/**
 * 
 */
UCLASS()
class TATOYS_API UNotifyHelper : public UObject
{
	GENERATED_BODY()
public:
	static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message,
										  bool bShowMsgAsWarning = true);
	static void ShowCornerPopupMessage(const FString& Message);
};
