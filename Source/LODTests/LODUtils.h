#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"


FStaticMeshComponentInstanceData *get_static_mesh_component_instance_data(UStaticMeshComponent *lod_object);

void increment_lod(UStaticMesh *lod_object);

void print_lod_info(const UStaticMesh *lod_object);

void increment_lod(UStaticMeshComponent *lod_object);//, TStructOnScope<FActorComponentInstanceData> &component_instance_data);

void print_lod_info(const UStaticMeshComponent *lod_object);

void reset_lod_level_to_zero(UStaticMesh *lod_object);

void reset_lod_level_to_zero(UStaticMeshComponent *lod_object);


TArray<UStaticMesh*> get_static_mesh_actors(UWorld *world);

TArray<UStaticMeshComponent*> get_static_mesh_components(UWorld *world);

void take_screenshot(const FString modelname, const FString details, const int32_t lod_idx);