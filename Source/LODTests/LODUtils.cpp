#include "LODUtils.h"

#include <cassert>

#include "r_defines.h"

#include "StaticMeshComponentLODInfo.h"


FStaticMeshComponentInstanceData *get_static_mesh_component_instance_data(UStaticMeshComponent *lod_object)
{
	TStructOnScope<FActorComponentInstanceData> component_instance_data = lod_object->GetComponentInstanceData();
	FStaticMeshComponentInstanceData *component_instance_data_cast = reinterpret_cast<FStaticMeshComponentInstanceData*>(component_instance_data.Get());
	return component_instance_data_cast;
}


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


	// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if >0, force to (ForcedLodModel-1)
	// so add 1 at the end
	const int32_t forced_lod_model = lod_object->ForcedLodModel;
	int32_t next_lod_model = ((forced_lod_model + 1) % NUM_LODS) + 1; 

	lod_object->SetForcedLodModel(next_lod_model);
}


void print_lod_info(const UStaticMesh *lod_object)
{
	const FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	int32_t lightmap_coordinate_index = lod_object->GetLightMapCoordinateIndex();
	int32_t lightmap_resolution = lod_object->GetLightMapResolution();
	float lightmap_uv_density = lod_object->GetLightmapUVDensity();
	int32_t lightmap_uv_version = lod_object->GetLightmapUVVersion();
	bool lods_share_static_lighting = static_mesh_render_data->bLODsShareStaticLighting;
	bool can_lods_share_static_lighting = lod_object->CanLODsShareStaticLighting();


	UE_LOG(LogTemp, Log, TEXT("Current first lod idx: %d\n"), static_mesh_render_data->CurrentFirstLODIdx);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap coordinate index: %d\n"), lightmap_coordinate_index);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap resolution: %d\n"), lightmap_resolution);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap uv density: %f\n"), lightmap_uv_density);
	UE_LOG(LogTemp, Log, TEXT("\tlightmap uv version: %d\n"), lightmap_uv_version);
	UE_LOG(LogTemp, Log, TEXT("\tlods share static lighting: %d\n"), lods_share_static_lighting);
	UE_LOG(LogTemp, Log, TEXT("\tcan lods share static lighting: %d\n"), can_lods_share_static_lighting);
}


void print_lod_info(const UStaticMeshComponent *lod_object)
{
	const int32_t forced_lod_model = lod_object->ForcedLodModel - 1;
	UE_LOG(LogTemp, Log, TEXT("Current first lod idx: %d\n"), forced_lod_model);
}


void reset_lod_level_to_zero(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	static_mesh_render_data->CurrentFirstLODIdx = 0;
}

// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if >0, force to (ForcedLodModel-1).
void reset_lod_level_to_zero(UStaticMeshComponent *lod_object)
{
	FStaticMeshComponentInstanceData* component_instance_data_cast = get_static_mesh_component_instance_data(lod_object);
	assert(component_instance_data_cast != nullptr);
	int32_t num_lightmaps = component_instance_data_cast->CachedStaticLighting.Num();

	for(int32_t i = 0; i < num_lightmaps; i++)
	{
		lod_object->LODData[i].MapBuildDataId = component_instance_data_cast->CachedStaticLighting[0];
	}

	lod_object->SetForcedLodModel(1);
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
					reset_lod_level_to_zero(static_mesh_component);
					relevant_static_mesh_components.Add(static_mesh_component);

					FString outstr = actor->GetActorLabel(false);
					UE_LOG(LogTemp, Log, TEXT("Actor name with static mesh: %s\n"), *outstr);

				}
			}

		}
	}

	return relevant_static_mesh_components;
}


void take_screenshot(const FString modelname, const FString details, const int32_t lod_idx)
{
	const FString ImageDirectory = FString::Printf(TEXT("%s/Screenshots/%s/"), *FPaths::ProjectDir(), *modelname);
	const FString filename = modelname + "_" + details + "_LOD_" + FString::FromInt(lod_idx);
	
	FScreenshotRequest::RequestScreenshot(ImageDirectory + filename, false, false);
}