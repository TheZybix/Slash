// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ToolTipComponent.generated.h"

class UToolTip;
/**
 * 
 */
UCLASS()
class SLASH_API UToolTipComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetToolTipText(const FText& ToolTipText);

protected:
	UPROPERTY()
	UToolTip* ToolTipWidget;
};
