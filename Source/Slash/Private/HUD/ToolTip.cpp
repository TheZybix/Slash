// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ToolTip.h"

#include "Components/TextBlock.h"

void UToolTip::SetToolTipInformation(const FText& NewText) const
{
	if (ToolTipInformation) ToolTipInformation->SetText(NewText);
}
