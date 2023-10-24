// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LODSelectorEccentricity.generated.h"

UCLASS()
class LODTESTS_API ALODSelectorEccentricity : public AActor
{
	GENERATED_BODY()
	
public:	
	TArray<AActor*> relevant_actors;
	
	
	size_t num_ticks = 0;


	// Sets default values for this actor's properties
	ALODSelectorEccentricity();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
