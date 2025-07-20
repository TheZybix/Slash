// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Math/UnitConversion.h"

void USlashOverlay::SetHealthBarPercentage(float Percentage)
{
	if (HealthProgressBar) HealthProgressBar->SetPercent(Percentage);
}

void USlashOverlay::SetStaminaBarPercentage(float Percentage)
{
	if (StaminaProgressBar) StaminaProgressBar->SetPercent(Percentage);
}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		/* Convert int32 to FString to FText to set the gold value in the overlay */
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
	}
}

void USlashOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		/* Convert int32 to FString to FText to set the gold value in the overlay */
		SoulsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
	}
}
