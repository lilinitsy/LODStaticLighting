#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"


FStaticMeshComponentInstanceData *get_static_mesh_component_instance_data(UStaticMeshComponent *lod_object);

FMeshMapBuildData *get_mesh_build_data(UStaticMeshComponent *lod_object, int32_t lod_idx);

void increment_lod(UStaticMesh *lod_object);

void print_lod_info(const UStaticMesh *lod_object);

// This is the non UFUNCTION version. Will have to copy paste this function into my experimental setups to use timer delegates, it seems
void increment_lod(UStaticMeshComponent *lod_object, const bool share_lightmap); //, TStructOnScope<FActorComponentInstanceData> &component_instance_data);

void print_lod_info(const UStaticMeshComponent *lod_object);

void set_lod_level(UStaticMeshComponent* lod_object, uint32_t lodlvl, const bool share_lightmap);

void reset_lod_level_to_zero(UStaticMesh *lod_object);

void reset_lod_level_to_zero(UStaticMeshComponent *lod_object);


TArray<UStaticMesh *> get_static_mesh_actors(UWorld *world);

TArray<UStaticMeshComponent *> get_static_mesh_components(UWorld *world);
TArray<UStaticMeshComponent *> get_static_mesh_components_unique_lightmaps(UWorld *world);


void take_screenshot(const FString modelname, const FString details, const int32_t lod_idx);