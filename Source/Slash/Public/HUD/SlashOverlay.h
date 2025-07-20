// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Set Health, Stamina, Gold and Souls values */
	void SetHealthBarPercentage(float Percentage);
	void SetStaminaBarPercentage(float Percentage);

	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	
private:
	/* Bind to the objects in the widget this is a parentclass to, names must match */
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsText;
};
