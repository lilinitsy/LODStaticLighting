// Fill out your copyright notice in the Description page of Project Settings.


#include "LODSelectorEccentricityInstanced.h"

#include "LODUtils.h"

// Sets default values
ALODSelectorEccentricityInstanced::ALODSelectorEccentricityInstanced()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALODSelectorEccentricityInstanced::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALODSelectorEccentricityInstanced::Tick(float DeltaTime)
{
	if(num_ticks == 300)
	{
		relevant_static_mesh_components = get_static_mesh_components(GetWorld());
		//relevant_static_mesh_components_instance_data = get_static_mesh_components_instance_data(relevant_static_mesh_components);
	}

	if(relevant_static_mesh_components.Num() > 0 && num_ticks > 300 && num_ticks % 120 == 0)
	{
		for(UStaticMeshComponent *static_mesh_component : relevant_static_mesh_components)
		{
			increment_lod(static_mesh_component);//, relevant_static_mesh_components_instance_data[0]);
			print_lod_info(static_mesh_component);
		}

	}

	num_ticks++;
}

