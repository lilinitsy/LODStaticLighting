#include "LODUtils.h"

#include "r_defines.h"


void increment_lod(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	const int32_t num_lods_available = lod_object->GetNumLODs();

	
	static_mesh_render_data->CurrentFirstLODIdx = num_lods_available >= (int) NUM_LODS ?
		(static_mesh_render_data->CurrentFirstLODIdx + 1) % NUM_LODS :
		(static_mesh_render_data->CurrentFirstLODIdx + 1) % num_lods_available;

}


void print_lod_info(const UStaticMesh *lod_object)
{
	const FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
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


TArray<UStaticMesh*> get_static_mesh_actors(UWorld *world)
{
	TArray<UStaticMesh*> relevant_static_mesh_components;
	ULevel *level = world->GetLevel(0);
	UE_LOG(LogTemp, Log, TEXT("Number of actors in this scene: %u\n"), level->Actors.Num());

	for(AActor* actor : level->Actors)
	{
		// Without this check, it'll segfault... either some actors don't have names, or some are nullptr's? 
		if(actor)
		{
			FString actor_name = actor->GetActorLabel(false);

			if(!actor_name.Contains(TEXT("Pawn")))
			{
				// Force all their static mesh components to be moveable
				TArray<UStaticMeshComponent*> static_mesh_components; // there should only be 1, but this returns an array
				actor->GetComponents<UStaticMeshComponent>(static_mesh_components);

				// Check that there is a static mesh
				for(UStaticMeshComponent *static_mesh_component : static_mesh_components)
				{
					// Add the first element only
					UStaticMesh *static_mesh = static_mesh_component->GetStaticMesh();
					relevant_static_mesh_components.Add(static_mesh);
					FString outstr = actor->GetActorLabel(false);
					UE_LOG(LogTemp, Log, TEXT("Actor name with static mesh: %s\n"), *outstr);

				}
			}

		}
	}

	return relevant_static_mesh_components;
}