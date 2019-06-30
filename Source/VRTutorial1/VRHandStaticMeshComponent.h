// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "VRHandStaticMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class VRTUTORIAL1_API UVRHandStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	UVRHandStaticMeshComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Grip();
	void ReleaseGrip();
	void PairController(UVRHandStaticMeshComponent* Controller);

private:
	UVRHandStaticMeshComponent* OtherController = nullptr;

	UFUNCTION()
		void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void CheckOverlap();
	bool CanClimb() const;
	bool bCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbingStartLocation;

	UPROPERTY(EditDefaultsOnly)
		class UHapticFeedbackEffect_Base* HapticEffect;
};
