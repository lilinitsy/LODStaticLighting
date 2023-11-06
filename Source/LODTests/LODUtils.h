#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"


void increment_lod(UStaticMesh *lod_object);

void print_lod_info(const UStaticMesh *lod_object);

void increment_lod(UStaticMeshComponent *lod_object);//, TStructOnScope<FActorComponentInstanceData> &component_instance_data);

void print_lod_info(const UStaticMeshComponent *lod_object);


TArray<UStaticMesh*> get_static_mesh_actors(UWorld *world);

TArray<UStaticMeshComponent*> get_static_mesh_components(UWorld *world);

//TArray<TStructOnScope<FActorComponentInstanceData>> get_static_mesh_components_instance_data(TArray<UStaticMeshComponent*> relevant_static_mesh_components);