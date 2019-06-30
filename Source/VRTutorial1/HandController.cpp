// Fill out your copyright notice in the Description page of Project Settings.


#include "HandController.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	MotionControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MotionControllerMesh"));
	MotionControllerMesh->SetupAttachment(MotionController);
}

void AHandController::SetHand(EControllerHand Hand)
{
	MotionController->SetTrackingSource(Hand);
	//MotionController->SetTrackingMotionSource(TEXT("Left"));
	MotionController->bDisplayDeviceModel = true;
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

