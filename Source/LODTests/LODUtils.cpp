#include "LODUtils.h"

#include <cassert>

#include "r_defines.h"

#include "StaticMeshComponentLODInfo.h"


void increment_lod(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	const int32_t num_lods_available = lod_object->GetNumLODs();

	
	static_mesh_render_data->CurrentFirstLODIdx = num_lods_available >= (int32_t) NUM_LODS ?
		(static_mesh_render_data->CurrentFirstLODIdx + 1) % NUM_LODS :
		(static_mesh_render_data->CurrentFirstLODIdx + 1) % num_lods_available;


}

void increment_lod(UStaticMeshComponent *lod_object)//, TStructOnScope<FActorComponentInstanceData> &component_instance_data)
{
	TStructOnScope<FActorComponentInstanceData> component_instance_data = lod_object->GetComponentInstanceData();

	FStaticMeshComponentInstanceData *component_instance_data_cast = reinterpret_cast<FStaticMeshComponentInstanceData*>(component_instance_data.Get());
	
	// These two below don't work, but might be safer?
	//FStaticMeshComponentInstanceData *cast_component_instance_data = component_instance_data.Cast();
	//FStaticMeshComponentInstanceData *cast_component_instance_data = TStructOnScope<FActorComponentInstanceData>::Cast<FStaticMeshComponentInstanceData*>(&component_instance_data);
	assert(component_instance_data_cast != nullptr);
	//lod_object->ApplyComponentInstanceData(component_instance_data_cast);
	int32_t num_lightmaps = component_instance_data_cast->CachedStaticLighting.Num();


	for(int32_t i = 0; i < num_lightmaps; i++)
	{
		lod_object->LODData[i].MapBuildDataId = component_instance_data_cast->CachedStaticLighting[0];
	}

	// Don't want to SetLODDataCount, because that will remove / add LOD's and then make them inaccessible
	// while still using the lightmap assigned for each lod.
	//lod_object->SetLODDataCount(1, 1);
	

	const int32_t forced_lod_model = lod_object->ForcedLodModel;
	int32_t next_lod_model = (forced_lod_model + 1) % NUM_LODS;

	lod_object->SetForcedLodModel(next_lod_model);

	
}





void print_lod_info(const UStaticMesh *lod_object)
{
	const FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	int32_t lightmap_coordinate_index = lod_object->GetLightMapCoordinateIndex();
	//FName lightmap_coordinate_index_name = lod_object->GetLightMapCoordinateIndexName();
	// ^ lightmap_coordinate_index_name is just printing "LightMapCoordinateIndex"
	int32_t lightmap_resolution = lod_object->GetLightMapResolution();
	float lightmap_uv_density = lod_object->GetLightmapUVDensity();
	int32_t lightmap_uv_version = lod_object->GetLightmapUVVersion();
	bool lods_share_static_lighting = static_mesh_render_data->bLODsShareStaticLighting;
	bool can_lods_share_static_lighting = lod_object->CanLODsShareStaticLighting();

	//FString lightmap_coordinate_index_name_string;
	//lightmap_coordinate_index_name.ToString(lightmap_coordinate_index_name_string);

	UE_LOG(LogTemp, Log, TEXT("Current first lod idx: %d\n"), static_mesh_render_data->CurrentFirstLODIdx);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap coordinate index: %d\n"), lightmap_coordinate_index);
	//UE_LOG(LogTemp, Log, TEXT("\tlightmap coordinate index name: %s\n"), *lightmap_coordinate_index_name_string);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap resolution: %d\n"), lightmap_resolution);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap uv density: %f\n"), lightmap_uv_density);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap uv version: %d\n"), lightmap_uv_version);
	UE_LOG(LogTemp, Log, TEXT("\tlods share static lighting: %d\n"), lods_share_static_lighting);
	UE_LOG(LogTemp, Log, TEXT("\tcan lods share static lighting: %d\n"), can_lods_share_static_lighting);
}


void print_lod_info(const UStaticMeshComponent *lod_object)
{
	const int32_t forced_lod_model = lod_object->ForcedLodModel;
	UE_LOG(LogTemp, Log, TEXT("Current first lod idx: %d\n"), forced_lod_model);
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





TArray<UStaticMeshComponent*> get_static_mesh_components(UWorld *world)
{
	TArray<UStaticMeshComponent*> relevant_static_mesh_components;
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
					relevant_static_mesh_components.Add(static_mesh_component);

					FString outstr = actor->GetActorLabel(false);
					UE_LOG(LogTemp, Log, TEXT("Actor name with static mesh: %s\n"), *outstr);

				}
			}

		}
	}

	return relevant_static_mesh_components;
}


/*TArray<TStructOnScope<FActorComponentInstanceData>> get_static_mesh_components_instance_data(TArray<UStaticMeshComponent*> relevant_static_mesh_components)
{
	TArray<TStructOnScope<FActorComponentInstanceData>> relevant_static_mesh_components_instance_data;

	for(UStaticMeshComponent *static_mesh_component : relevant_static_mesh_components)
	{
		TStructOnScope<FActorComponentInstanceData> component_instance_data = static_mesh_component->GetComponentInstanceData();
		relevant_static_mesh_components_instance_data.Add(component_instance_data);
	}

	return relevant_static_mesh_components_instance_data;
}*/