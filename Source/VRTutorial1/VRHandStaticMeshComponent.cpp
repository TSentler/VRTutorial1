// Fill out your copyright notice in the Description page of Project Settings.


#include "VRHandStaticMeshComponent.h"
#include "MotionControllerComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
UVRHandStaticMeshComponent::UVRHandStaticMeshComponent()
{
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UVRHandStaticMeshComponent::BeginPlay()
{
	OnComponentBeginOverlap.AddDynamic(this, &UVRHandStaticMeshComponent::ComponentBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UVRHandStaticMeshComponent::ComponentEndOverlap);
}

void UVRHandStaticMeshComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType,ThisTickFunction);

	if (bIsClimbing)
	{
		FVector HandControllerDelta = GetComponentLocation() - ClimbingStartLocation;
		//GetAttachParentActor
		GetOwner()->AddActorWorldOffset(-HandControllerDelta);
	}
}


void UVRHandStaticMeshComponent::Grip()
{
	if (!bCanClimb)
		return;

	if (!bIsClimbing)
	{
		bIsClimbing = true;
		ClimbingStartLocation = GetComponentLocation();

		OtherController->bIsClimbing = false;

		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character != nullptr)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}
}

void UVRHandStaticMeshComponent::ReleaseGrip()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;

		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character != nullptr)
		{
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}
}


void UVRHandStaticMeshComponent::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckOverlap();
}
void UVRHandStaticMeshComponent::ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanClimb = CanClimb();
}

void UVRHandStaticMeshComponent::CheckOverlap()
{
	auto bNewCanClimb = CanClimb();
	if (!bCanClimb && bNewCanClimb) //Just can climb
	{
		//APawn* Pawn = Cast<APawn>(GetAttachParentActor());
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (Pawn != nullptr)
		{

			APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
			if (Controller != nullptr)
			{
				UMotionControllerComponent* MotionController = Cast<UMotionControllerComponent>(GetAttachParent());
				Controller->PlayHapticEffect(HapticEffect, MotionController->GetTrackingSource());
			}
		}
	}
	bCanClimb = bNewCanClimb;
}

bool UVRHandStaticMeshComponent::CanClimb() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
		if (OverlappingActor->ActorHasTag(TEXT("Climbable")))
			return true;

	return false;
}

void UVRHandStaticMeshComponent::PairController(UVRHandStaticMeshComponent* Controller)
{
	OtherController = Controller;
}