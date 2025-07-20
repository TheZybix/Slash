// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ToolTipComponent.h"

#include "Components/TextBlock.h"
#include "HUD/ToolTip.h"

void UToolTipComponent::SetToolTipText(const FText& ToolTipText)
{
	if (ToolTipWidget == nullptr)
	{
		ToolTipWidget = Cast<UToolTip>(GetUserWidgetObject());
	}

	if (ToolTipWidget)
	{
		ToolTipWidget->SetToolTipInformation(ToolTipText);
	}
}
