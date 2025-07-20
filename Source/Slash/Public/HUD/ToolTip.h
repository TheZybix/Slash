// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTip.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetToolTipInformation(const FText& NewText) const;

private:
	UPROPERTY (meta = (BindWidget))
	class UTextBlock* ToolTipInformation;
};
