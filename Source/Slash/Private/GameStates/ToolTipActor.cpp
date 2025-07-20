// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStates/ToolTipActor.h"
#include "Characters/SlashCharacter.h"
#include "Components/BoxComponent.h"
#include "HUD/ToolTipComponent.h"

AToolTipActor::AToolTipActor()
{
	PrimaryActorTick.bCanEverTick = false;
	ToolTipBoxComponent = CreateDefaultSubobject<UBoxComponent>("ToolTipComponent");
	ToolTipBoxComponent->SetCollisionObjectType(ECC_WorldStatic);
	ToolTipBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ToolTipBoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	ToolTipComponent = CreateDefaultSubobject<UToolTipComponent>(TEXT("Tool Tip"));
	ToolTipComponent->SetupAttachment(GetRootComponent());
}

void AToolTipActor::BeginPlay()
{
	Super::BeginPlay();
	ToolTipBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AToolTipActor::OnBoxOverlap);
	ToolTipBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AToolTipActor::OnBoxEndOverlap);

	/* Initialize tooltip and hide it until player overlaps with it */
	SetUpToolTipComponent();
	HideTooltip();
}

void AToolTipActor::SetUpToolTipComponent()
{
	/* If the component is valid and the actor associated with the tooltip (for example combat tooltips for a sword), the text in the widget will be set and it will be attached to the ToolTipTargetActor
		There's an offset also instance editable to finetune the positioning of the tooltip	 */
	if (ToolTipComponent && ToolTipTargetActor)
	{
		ToolTipComponent->SetToolTipText(ToolTipText);
		ToolTipComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		
		ToolTipLocation = ToolTipTargetActor->GetActorLocation() + ToolTipOffset;
		ToolTipComponent->SetWorldLocation(ToolTipLocation);
	}
	else UE_LOG(LogTemp, Error, TEXT("ToolTip or Actor could not be found"));
}

void AToolTipActor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/* OnBoxOverlap shows the player the tooltip that's been set in the actor's instance derived from this class in the world*/
	ASlashCharacter* PlayerCharacter = Cast<ASlashCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		SetUpToolTipComponent();
		if (bShowTooltip) ShowTooltip();
		if (bIsVictory) PlayerCharacter->bCanWin = true; // Checks if this tooltip is also a victory condition for the player
	}
}

void AToolTipActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/* Once the player stops overlapping with the box collision, the tooltip disappears, and if it's a victory condition, it's set back to false */
	ASlashCharacter* PlayerCharacter = Cast<ASlashCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		HideTooltip();
		if (!bRemainVisible) bShowTooltip = false; // Can be set in the individual instances, allows tooltips to not become visible again on successive overlaps with the box collisions
		if (bIsVictory) PlayerCharacter->bCanWin = false;
	}

}

void AToolTipActor::ShowTooltip()
{
	if (ToolTipComponent)
	{
		ToolTipComponent->SetVisibility(true);
	}
}

void AToolTipActor::HideTooltip()
{
	if (ToolTipComponent)
	{
		ToolTipComponent->SetVisibility(false);
	}
}