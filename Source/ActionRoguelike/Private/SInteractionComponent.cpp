// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract() const
{
	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	APawn* Owner = Cast<APawn>(GetOwner());

	FVector EyeLocation;
	FRotator EyeRotation;
	Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    const FVector End = EyeLocation + EyeRotation.Vector() * 1000;

    TArray<FHitResult> Hits;
	//GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, CollisionObjectQueryParams);

	GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, CollisionObjectQueryParams, FCollisionShape::MakeSphere(35.0f));

	for (FHitResult Hit : Hits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				ISGameplayInterface::Execute_Interact(HitActor, Owner);
				DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 35.0f, 32, FColor::Green, false, 2.0f, 0, 1.0f);
				break;
			}
		}
	}
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

