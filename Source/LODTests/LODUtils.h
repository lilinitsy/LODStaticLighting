#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"


void increment_lod(UStaticMesh *lod_object);

void print_lod_info(const UStaticMesh *lod_object);

TArray<UStaticMesh*> get_static_mesh_actors(UWorld *world);
