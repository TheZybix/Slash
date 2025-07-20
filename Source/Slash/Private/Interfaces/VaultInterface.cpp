// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/VaultInterface.h"

// Add default functionality here for any IVaultInterface functions that are not pure virtual.
void IVaultInterface::SetVaultWarpLocations(const FVector Origin, const FVector Start, const FVector Middle, const FVector End)
{
}

void IVaultInterface::CanVault(const bool bVault)
{
}

bool IVaultInterface::CanUpdateWarpLocations()
{
	return false;
}
