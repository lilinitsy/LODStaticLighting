// Fill out your copyright notice in the Description page of Project Settings.


#include "LODSelectorEccentricity.h"

#include "LODUtils.h"

// Sets default values
ALODSelectorEccentricity::ALODSelectorEccentricity()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALODSelectorEccentricity::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALODSelectorEccentricity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(num_ticks == 300)
	{
		relevant_static_meshes = get_static_mesh_actors(GetWorld());
	}

	if(relevant_static_meshes.Num() > 0 && num_ticks > 300 && num_ticks % 120 == 0)
	{
		for(UStaticMesh *static_mesh : relevant_static_meshes)
		{
			increment_lod(static_mesh);
		}
	}

	num_ticks++;
}
