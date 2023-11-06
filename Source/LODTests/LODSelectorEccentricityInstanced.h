// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LODSelectorEccentricityInstanced.generated.h"

UCLASS()
class LODTESTS_API ALODSelectorEccentricityInstanced : public AActor
{
	GENERATED_BODY()
	
public:	
	TArray<UStaticMeshComponent*> relevant_static_mesh_components;
	TArray<TStructOnScope<FActorComponentInstanceData>> relevant_static_mesh_components_instance_data;

	size_t num_ticks = 0;


	// Sets default values for this actor's properties
	ALODSelectorEccentricityInstanced();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
