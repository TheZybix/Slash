// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacles/VaultObstacle.h"

#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/VaultInterface.h"

// Sets default values
AVaultObstacle::AVaultObstacle()
{
	/* Basic setup for all the components */
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootComponent = ItemMesh;

	FrontBoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("FrontBoxCollision"));
	FrontBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FrontBoxCollision->SetCollisionObjectType(ECC_WorldStatic);
	FrontBoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	FrontBoxCollision->SetupAttachment(ItemMesh);
	
	BackBoxCollision = CreateDefaultSubobject<UBoxComponent>(FName("BackBoxCollision"));
	BackBoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BackBoxCollision->SetCollisionObjectType(ECC_WorldStatic);
	BackBoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	BackBoxCollision->SetupAttachment(ItemMesh);

	WorldOriginFront = CreateDefaultSubobject<USceneComponent>(TEXT("OriginFront"));
	WorldOriginFront->SetupAttachment(ItemMesh);
	WorldOriginBack = CreateDefaultSubobject<USceneComponent>(TEXT("OriginBack"));
	WorldOriginBack->SetupAttachment(ItemMesh);
	WorldVaultFront = CreateDefaultSubobject<USceneComponent>(TEXT("VaultFront"));
	WorldVaultFront->SetupAttachment(ItemMesh);
	WorldVaultMiddle = CreateDefaultSubobject<USceneComponent>(TEXT("VaultMiddle"));
	WorldVaultMiddle->SetupAttachment(ItemMesh);
	WorldVaultBack = CreateDefaultSubobject<USceneComponent>(TEXT("VaultBack"));
	WorldVaultBack->SetupAttachment(ItemMesh);

	NavBlocker = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavBlocker"));
}


void AVaultObstacle::BeginPlay()
{
	/* Set up ComponentOverlap-functions */
	Super::BeginPlay();
	FrontBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AVaultObstacle::OnFrontBoxOverlap);
	FrontBoxCollision->OnComponentEndOverlap.AddDynamic(this, &AVaultObstacle::OnFrontBoxEndOverlap);
	
	BackBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AVaultObstacle::OnBackBoxOverlap);
	BackBoxCollision->OnComponentEndOverlap.AddDynamic(this, &AVaultObstacle::OnBackBoxEndOverlap);
}

void AVaultObstacle::SetTargetWarpTargets(AActor* OtherActor)
{
	/* Cast to the vault interface and if it's valid, update warp targets and vault-bool in the other actor */
	IVaultInterface* VaultInterface = Cast<IVaultInterface>(OtherActor);
	if (VaultInterface)
	{
		VaultInterface->SetVaultWarpLocations(Origin, StartPosition, MiddlePosition, EndPosition);
		VaultInterface->CanVault(AllowVault);
	}
}

void AVaultObstacle::DisableVault(AActor* OtherActor)
{
	/* Also cast to vault interface and disable actor's ability to vault */
	IVaultInterface* VaultInterface = Cast<IVaultInterface>(OtherActor);
	if (VaultInterface)
	{
		AllowVault = false;
		VaultInterface->CanVault(AllowVault);
	}
}

void AVaultObstacle::OnFrontBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* Currently only the player is supposed to be able to vault, but this tag can be adjusted once enemies should also vault */
	if (OtherActor->ActorHasTag("Player"))
	{
		/* Set the motion warp targets and allow character to vault */
		SetMotionTargets(WorldOriginFront->GetComponentLocation(), WorldVaultFront->GetComponentLocation(), WorldVaultMiddle->GetComponentLocation(), WorldOriginBack->GetComponentLocation());
		AllowVault = true;
		SetTargetWarpTargets(OtherActor);
	}
		
}

void AVaultObstacle::OnFrontBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Prevent characters from performing the vault action once they stop overlapping with the box collision */
	DisableVault(OtherActor);
}

void AVaultObstacle::OnBackBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* Currently only the player is supposed to be able to vault, but this tag can be adjusted once enemies should also vault */
	if (OtherActor->ActorHasTag("Player"))
	{
		/* Set the motion warp targets and allow character to vault */
		SetMotionTargets(WorldOriginBack->GetComponentLocation(), WorldVaultBack->GetComponentLocation(), WorldVaultMiddle->GetComponentLocation(), WorldOriginFront->GetComponentLocation());
		AllowVault = true;
		SetTargetWarpTargets(OtherActor);
	}
}

void AVaultObstacle::OnBackBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Prevent characters from performing the vault action once they stop overlapping with the box collision */
	DisableVault(OtherActor);
}

void AVaultObstacle::SetMotionTargets(FVector Start, FVector Front, FVector Middle, FVector End)
{
	/* Set the motion targets based on whether the character overlapped with the front or back box collision, so they vault in the appropriate direction */
	Origin = Start;
	StartPosition = Front; 
	MiddlePosition = Middle,
	EndPosition = End;
}