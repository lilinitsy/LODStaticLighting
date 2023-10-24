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
	Super::Tick(DeltaTime);

	if(num_ticks % 180 == 0)
	{
		increment_lod(lod_object);
		print_lod_info(lod_object);
	}

	num_ticks++;
}


void ALODSelector::get_lod_object_data()
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();

	static_mesh_render_data->CurrentFirstLODIdx = (static_mesh_render_data->CurrentFirstLODIdx + 1) % NUM_LODS;
	int32_t lightmap_coordinate_index = lod_object->GetLightMapCoordinateIndex();
	int32_t lightmap_resolution = lod_object->GetLightMapResolution();
	float lightmap_uv_density = lod_object->GetLightmapUVDensity();
	bool lods_share_static_lighting = static_mesh_render_data->bLODsShareStaticLighting;

	/*UE_LOG(LogTemp Log, TEXT("Current LOD: %d\n"), static_mesh_render_data->CurrentFirstLODIdx);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap coordinate index: %d\n"), lightmap_coordinate_index);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap resolution: %d\n"), lightmap_resolution);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap uv density: %f\n"), lightmap_uv_density);
	UE_LOG(LogTemp, Log, TEXT("\tlods share static lighting: %d\n"), lods_share_static_lighting);
	*/


}