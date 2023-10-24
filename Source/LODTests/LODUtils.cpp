#include "LODUtils.h"

#include "r_defines.h"


void increment_lod(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	static_mesh_render_data->CurrentFirstLODIdx = (static_mesh_render_data->CurrentFirstLODIdx + 1) % NUM_LODS;
}


void print_lod_info(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	int32_t lightmap_coordinate_index = lod_object->GetLightMapCoordinateIndex();
	int32_t lightmap_resolution = lod_object->GetLightMapResolution();
	float lightmap_uv_density = lod_object->GetLightmapUVDensity();
	bool lods_share_static_lighting = static_mesh_render_data->bLODsShareStaticLighting;

	UE_LOG(LogTemp, Log, TEXT("Current first lod idx: %d\n"), static_mesh_render_data->CurrentFirstLODIdx);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap coordinate index: %d\n"), lightmap_coordinate_index);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap resolution: %d\n"), lightmap_resolution);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap uv density: %f\n"), lightmap_uv_density);
	UE_LOG(LogTemp, Log, TEXT("\tlods share static lighting: %d\n"), lods_share_static_lighting);
}


TArray<AActor*> get_static_mesh_actors(UWorld *world)
{
	TArray<AActor*> relevant_actors;
	ULevel *level = world->GetLevel(0);
	for(AActor* actor : level->Actors)
	{
		// Without this check, it'll segfault... either some actors don't have names, or some are nullptr's? 
		if(actor)
		{
			FString actor_name = actor->GetActorLabel(false);

			// Check for spheres, but not sphere reflection captures
			if(actor_name.Contains(TEXT("sphere")) && !actor_name.Contains(TEXT("reflection")) && !actor_name.Contains(TEXT("sky")))
			{
				relevant_actors.Add(actor);

				// Force all their static mesh components to be moveable
				TArray<UStaticMeshComponent*> static_mesh_components; // there should only be 1, but this returns an array
				actor->GetComponents<UStaticMeshComponent>(static_mesh_components);
				UStaticMeshComponent *component = static_mesh_components[0];
				component->SetMobility(EComponentMobility::Movable);
			}
		}
	}

	return relevant_actors;
}