// Fill out your copyright notice in the Description page of Project Settings.


#include "LODSelector.h"

#include "LODUtils.h"
#include "r_defines.h"

// Sets default values
ALODSelector::ALODSelector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALODSelector::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALODSelector::Tick(float DeltaTime)
{
	if(num_ticks == 300)
	{
		relevant_static_mesh_components = get_static_mesh_components_unique_lightmaps(GetWorld());
	}

	if(relevant_static_mesh_components.Num() > 0 && num_ticks > 300 && num_ticks % 120 == 0)
	{
		for(UStaticMeshComponent *static_mesh_component : relevant_static_mesh_components)
		{
			increment_lod(static_mesh_component);
			print_lod_info(static_mesh_component);
			if(TAKE_SCREENSHOTS)
			{
				take_screenshot(modelname, details, static_mesh_component->ForcedLodModel - 1);
			}
		}
	}

	num_ticks++;
}