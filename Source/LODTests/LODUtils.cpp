#include "LODUtils.h"

#include <cassert>

#include "r_defines.h"

#include "StaticMeshComponentLODInfo.h"

#include "Engine/MapBuildDataRegistry.h"
#include "Lightmap.h"
#include "Rendering/Texture2DResource.h"


FStaticMeshComponentInstanceData *get_static_mesh_component_instance_data(UStaticMeshComponent *lod_object)
{
	TStructOnScope<FActorComponentInstanceData> component_instance_data      = lod_object->GetComponentInstanceData();
	FStaticMeshComponentInstanceData           *component_instance_data_cast = reinterpret_cast<FStaticMeshComponentInstanceData *>(component_instance_data.Get());
	return component_instance_data_cast;
}


void increment_lod(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	const int32_t          num_lods_available      = lod_object->GetNumLODs();


	static_mesh_render_data->CurrentFirstLODIdx = num_lods_available >= (int32_t) NUM_LODS ?
													  (static_mesh_render_data->CurrentFirstLODIdx + 1) % NUM_LODS :
													  (static_mesh_render_data->CurrentFirstLODIdx + 1) % num_lods_available;
}

void increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap) //, TStructOnScope<FActorComponentInstanceData> &component_instance_data)
{
	// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if > 0, force to (ForcedLodModel-1)
	// so add 1 at the end
	const int32_t forced_lod_model      = lod_object->ForcedLodModel - 1;
	int32_t       forced_next_lod_model = ((forced_lod_model + 1) % NUM_LODS) + 1;
	int32_t       next_lod_level        = forced_next_lod_model - 1;

	lod_object->SetForcedLodModel(forced_next_lod_model);

	if(share_lightmap)
	{
		FMeshMapBuildData *meshmap_build_data      = get_mesh_build_data(lod_object, next_lod_level);
		FMeshMapBuildData *meshmap_build_data_at_0 = get_mesh_build_data(lod_object, 0);
		FLightMap2D       *lightmap                = meshmap_build_data_at_0->LightMap->GetLightMap2D();

		if(MIPMAP_SHARED_LIGHTMAP && lightmap)
		{
			if(lightmap->Textures[0])
			{
				lightmap->Textures[0]->LODBias = next_lod_level;
			}

			if(lightmap->Textures[1])
			{
				lightmap->Textures[1]->LODBias = next_lod_level;
			}
		}
	}
}

void set_lod_level(UStaticMeshComponent *lod_object, uint32_t lodlvl, const bool share_lightmap)
{
	lodlvl                        = std::clamp(lodlvl, (uint32_t) 0, (uint32_t) NUM_LODS);
	int32_t forced_next_lod_model = (lodlvl % NUM_LODS) + 1;
	int32_t next_lod_level        = forced_next_lod_model - 1;
	lod_object->SetForcedLodModel(forced_next_lod_model);

	if(share_lightmap)
	{
		FMeshMapBuildData *meshmap_build_data = get_mesh_build_data(lod_object, next_lod_level);
		FLightMap2D       *lightmap           = (meshmap_build_data && meshmap_build_data->LightMap) ?
													meshmap_build_data->LightMap->GetLightMap2D() :
													nullptr;

		if(MIPMAP_SHARED_LIGHTMAP && lightmap)
		{
			if(lightmap->Textures[0])
			{
				lightmap->Textures[0]->LODBias = next_lod_level;
			}

			if(lightmap->Textures[1])
			{
				lightmap->Textures[1]->LODBias = next_lod_level;
			}
		}
	}
}


void print_lod_info(const UStaticMesh *lod_object)
{
	const FStaticMeshRenderData *static_mesh_render_data        = lod_object->GetRenderData();
	int32_t                      lightmap_coordinate_index      = lod_object->GetLightMapCoordinateIndex();
	int32_t                      lightmap_resolution            = lod_object->GetLightMapResolution();
	float                        lightmap_uv_density            = lod_object->GetLightmapUVDensity();
	int32_t                      lightmap_uv_version            = lod_object->GetLightmapUVVersion();
	bool                         lods_share_static_lighting     = static_mesh_render_data->bLODsShareStaticLighting;
	bool                         can_lods_share_static_lighting = lod_object->CanLODsShareStaticLighting();


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


FMeshMapBuildData *get_mesh_build_data(UStaticMeshComponent *lod_object, int32_t lod_idx)
{
	AActor                      *owner    = lod_object->GetOwner();
	FStaticMeshComponentLODInfo &lod_info = lod_object->LODData[lod_idx];

	if(owner)
	{
		ULevel *owner_level = owner->GetLevel();

		if(owner_level && owner_level->OwningWorld)
		{
			ULevel                *active_lighting_scenario = owner_level->OwningWorld->GetActiveLightingScenario();
			UMapBuildDataRegistry *map_build_data           = nullptr;

			if(active_lighting_scenario && active_lighting_scenario->MapBuildData)
			{
				map_build_data = active_lighting_scenario->MapBuildData;
			}
			else if(owner_level->MapBuildData)
			{
				map_build_data = owner_level->MapBuildData;
			}

			if(map_build_data)
			{
				return map_build_data->GetMeshBuildData(lod_info.MapBuildDataId);
			}
		}
	}

	return nullptr;
}


void reset_lod_level_to_zero(UStaticMesh *lod_object)
{
	FStaticMeshRenderData *static_mesh_render_data = lod_object->GetRenderData();
	static_mesh_render_data->CurrentFirstLODIdx    = 0;
}

// UStaticMeshComponent::ForcedLodModel: If 0, auto-select LOD level. if >0, force to (ForcedLodModel-1).
void reset_lod_level_to_zero(UStaticMeshComponent *lod_object)
{
	FStaticMeshComponentInstanceData *component_instance_data_cast = get_static_mesh_component_instance_data(lod_object);
	assert(component_instance_data_cast != nullptr);

	FMeshMapBuildData *meshmap_build_data_at_0 = get_mesh_build_data(lod_object, 0);
	FLightMap2D       *lightmap_at_0           = meshmap_build_data_at_0->LightMap->GetLightMap2D();
	assert(lightmap_at_0 != nullptr);

	int32_t num_lods = lod_object->LODData.Num();

	for(int32_t i = 1; i < num_lods; i++)
	{
		FMeshMapBuildData *meshmap_build_data = get_mesh_build_data(lod_object, i);

		if(meshmap_build_data)
		{
			meshmap_build_data->LightMap = lightmap_at_0;
		}

		meshmap_build_data = meshmap_build_data_at_0;
	}
}

TArray<UStaticMesh *> get_static_mesh_actors(UWorld *world)
{
	TArray<UStaticMesh *> relevant_static_mesh_components;
	ULevel               *level = world->GetLevel(0);

	for(AActor *actor : level->Actors)
	{
		// Without this check, it'll segfault... either some actors don't have names, or some are nullptr's?
		if(actor)
		{
			FString actor_name = actor->GetActorLabel(false);

			if(!actor_name.Contains(TEXT("Pawn")))
			{
				// Force all their static mesh components to be moveable
				TArray<UStaticMeshComponent *> static_mesh_components; // there should only be 1, but this returns an array
				actor->GetComponents<UStaticMeshComponent>(static_mesh_components);

				// Check that there is a static mesh
				for(UStaticMeshComponent *static_mesh_component : static_mesh_components)
				{
					UStaticMesh *static_mesh = static_mesh_component->GetStaticMesh();
					relevant_static_mesh_components.Add(static_mesh);
					FString outstr = actor->GetActorLabel(false);
				}
			}
		}
	}

	return relevant_static_mesh_components;
}


TArray<UStaticMeshComponent *> get_static_mesh_components(UWorld *world)
{
	TArray<UStaticMeshComponent *> relevant_static_mesh_components;
	ULevel                        *level = world->GetLevel(0);

	for(AActor *actor : level->Actors)
	{
		// Without this check, it'll segfault... either some actors don't have names, or some are nullptr's?
		if(actor)
		{
			FString actor_name = actor->GetActorLabel(false);

			if(!actor_name.Contains(TEXT("Pawn")) && !actor_name.Contains(TEXT("UNIQUE")) && actor_name.Contains(TEXT("REUSE")))
			{
				// Force all their static mesh components to be moveable
				TArray<UStaticMeshComponent *> static_mesh_components; // there should only be 1, but this returns an array
				actor->GetComponents<UStaticMeshComponent>(static_mesh_components);

				// Check that there is a static mesh
				for(UStaticMeshComponent *static_mesh_component : static_mesh_components)
				{
					// Add the first element only
					reset_lod_level_to_zero(static_mesh_component);
					relevant_static_mesh_components.Add(static_mesh_component);

					FString outstr = actor->GetActorLabel(false);
				}
			}
		}
	}

	return relevant_static_mesh_components;
}


TArray<UStaticMeshComponent *> get_static_mesh_components_unique_lightmaps(UWorld *world)
{
	TArray<UStaticMeshComponent *> relevant_static_mesh_components;
	ULevel                        *level = world->GetLevel(0);

	for(AActor *actor : level->Actors)
	{
		// Without this check, it'll segfault... either some actors don't have names, or some are nullptr's?
		if(actor)
		{
			FString actor_name = actor->GetActorLabel(false);

			if(!actor_name.Contains(TEXT("Pawn")) && actor_name.Contains(TEXT("UNIQUE")))
			{
				// Force all their static mesh components to be moveable
				TArray<UStaticMeshComponent *> static_mesh_components; // there should only be 1, but this returns an array
				actor->GetComponents<UStaticMeshComponent>(static_mesh_components);

				// Check that there is a static mesh
				for(UStaticMeshComponent *static_mesh_component : static_mesh_components)
				{
					// Add the first element only
					relevant_static_mesh_components.Add(static_mesh_component);

					FString outstr = actor->GetActorLabel(false);
				}
			}
		}
	}

	return relevant_static_mesh_components;
}




void take_screenshot(const FString modelname, const FString details, const int32_t lod_idx)
{
	const FString ImageDirectory = FString::Printf(TEXT("%s/Screenshots/%s/"), *FPaths::ProjectDir(), *modelname);
	const FString filename       = modelname + "_" + details + "_LOD_" + FString::FromInt(lod_idx);

	FScreenshotRequest::RequestScreenshot(ImageDirectory + filename, false, false);
}